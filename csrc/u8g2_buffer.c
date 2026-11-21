/*
 * ============================================================================
 * 【中文导读】u8g2_buffer.c —— 帧缓冲管理（读 u8g2 的起点）
 * ----------------------------------------------------------------------------
 * u8g2 的绘制模型：所有画图函数只改 RAM 里的帧缓冲（128x64/8=1KB），
 * u8g2_SendBuffer() 才整帧发给屏幕。本文件：缓冲分配(u8g2_SetupBuffer)、
 * 清屏、FirstPage/NextPage 分页模式（省 RAM 的逐页绘制）。
 * ============================================================================
 */

/* 

  u8g2_buffer.c 

  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright (c) 2016, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  

*/

#include "u8g2.h"
#include <string.h>

/*============================================*/
/**
 * @brief 把 RAM 帧缓冲整体清零（所有字节置 0），只清 RAM，不碰显示 RAM
 * @param  u8g2  上下文，依赖 u8g2_SetupBuffer 已装好的 tile_buf_ptr / tile_buf_height
 * @note   调用时机：用户手动清屏，或 FirstPage/NextPage 在 is_auto_page_clear=1 时自动调用。
 *         清完后屏幕上仍是旧画面——真正可见要等 u8g2_send_buffer()/u8g2_UpdateDisplay() 发出去。
 */
void u8g2_ClearBuffer(u8g2_t *u8g2)
{
  size_t cnt;
  cnt = u8g2_GetU8x8(u8g2)->display_info->tile_width;	// 一"tile 行"的字节数 = 宽像素/8（128 宽 = 16）
  cnt *= u8g2->tile_buf_height;				// 缓冲的 tile 行数：页模式 1/2 行，全缓冲 = 满屏行数
  cnt *= 8;						// 每 tile 竖跨 8 像素行，即 8 字节
  memset(u8g2->tile_buf_ptr, 0, cnt);			// 典型 128x64 全缓冲：16*8*8 = 1024 字节
}

/*============================================*/

/**
 * @brief 把缓冲里第 src_tile_row 个 tile 行（8 像素行）发送到显示 RAM 的 dest_tile_row 行
 * @param  src_tile_row   缓冲内的 tile 行号（0 = 缓冲顶部）
 * @param  dest_tile_row  目标显示 RAM 的 tile 行号（页模式下 = tile_curr_row + src_tile_row）
 * @note   这是"缓冲 → 硬件"的最小搬运单元：以 tile 行为单位调 u8x8_DrawTile，
 *         走 cad→byte 消息链（U8X8_MSG_DISPLAY_DRAW_TILE）落到控制器。
 *         仅支持横排字节（horizontal bytes，1 像素 = 1 bit 向右增长）架构的屏；
 *         竖排架构（ST7920 等）另有专用 ll_hvline，发送端也在各自驱动里处理。
 */
static void u8g2_send_tile_row(u8g2_t *u8g2, uint8_t src_tile_row, uint8_t dest_tile_row)
{
  uint8_t *ptr;
  uint16_t offset;
  uint8_t w;

  w = u8g2_GetU8x8(u8g2)->display_info->tile_width;	// 每行 tile 数（128 宽 = 16）
  offset = src_tile_row;					// 缓冲内偏移 = 行号 × 每行 tile 数 × 8 字节
  ptr = u8g2->tile_buf_ptr;
  offset *= w;
  offset *= 8;
  ptr += offset;						// 定位到本 tile 行的起始字节
  u8x8_DrawTile(u8g2_GetU8x8(u8g2), 0, dest_tile_row, w, ptr);	// 一次发 w 个 tile（16 字节）
}

/* 
  write the buffer to the display RAM. 
  For most displays, this will make the content visible to the user.
  Some displays (like the SSD1606) require a u8x8_RefreshDisplay()
*/
static void u8g2_send_buffer(u8g2_t *u8g2) U8X8_NOINLINE;
/**
 * @brief 把整个缓冲逐 tile 行写入显示 RAM（页模式只写当前页对应的窗口）
 * @param  u8g2  上下文；tile_curr_row 决定目标起始行
 * @note   核心搬运循环：src 从缓冲第 0 行起，dest 从 tile_curr_row 起，二者同步推进。
 *         do-while 保证至少发一行；两侧任一到达上限即停——
 *         全缓冲模式 src_max == dest_max 一次发完；页模式 tile_buf_height < tile_height，
 *         每轮只发本页覆盖的那几行（dest_max 兜底防止最后几页越界写入显示 RAM 之外）。
 */
static void u8g2_send_buffer(u8g2_t *u8g2)
{
  uint8_t src_row;
  uint8_t src_max;
  uint8_t dest_row;
  uint8_t dest_max;

  src_row = 0;
  src_max = u8g2->tile_buf_height;			// 缓冲高度（tile 行）
  dest_row = u8g2->tile_curr_row;			// 当前页起始行（FirstPage/NextPage 推进）
  dest_max = u8g2_GetU8x8(u8g2)->display_info->tile_height;	// 显示 RAM 总 tile 行数

  do
  {
    u8g2_send_tile_row(u8g2, src_row, dest_row);	// 缓冲第 src_row 行 → 显示第 dest_row 行
    src_row++;
    dest_row++;
  } while( src_row < src_max && dest_row < dest_max );	// 任一侧走完就停，天然处理非 8 倍数高度屏
}

/* same as u8g2_send_buffer but also send the DISPLAY_REFRESH message (used by SSD1606) */
/**
 * @brief 对外 API：发整帧缓冲到显示 RAM，随后补发刷新消息（U8X8_MSG_DISPLAY_REFRESH）
 * @param  u8g2  上下文
 * @note   普通 OLED/LCD（SSD1306 等）控制器 RAM 即显存，写完即见；电子纸（SSD1606 等）
 *         必须靠末尾的 RefreshDisplay 触发一次刷屏（耗时数百 ms）——所以纯电子纸场景
 *         要少调用本函数，合并多次绘制后一次发送。
 *         "绘图循环四步曲"中的第三步：ClearBuffer → 画图 → SendBuffer → 延时。
 */
void u8g2_SendBuffer(u8g2_t *u8g2)
{
  u8g2_send_buffer(u8g2);				// 逐 tile 行拷贝缓冲 → 显示 RAM
  u8x8_RefreshDisplay( u8g2_GetU8x8(u8g2) );		// 电子纸需要；其他控制器此消息一般为空操作
}

/*============================================*/
/**
 * @brief 设置当前页起始 tile 行，并联动重算裁剪窗口/尺寸
 * @param  row  新的 tile 行号（0..tile_height-1，步长为 tile_buf_height）
 * @note   关键副作用：update_dimension 依 tile_curr_row 重算 buf_y0/buf_y1（本页在
 *         缓冲坐标里的纵向范围），update_page_win 再把它映射成 user_x0..y1 窗口；
 *         之后所有 DrawXxx 的裁剪都基于这个窗口——页模式下"画到屏外"的图元
 *         就是在这里被挡掉的。
 */
void u8g2_SetBufferCurrTileRow(u8g2_t *u8g2, uint8_t row)
{
  u8g2->tile_curr_row = row;		// 记录本页对应显示 RAM 的起始 tile 行
  u8g2->cb->update_dimension(u8g2);	// 重算 pixel_curr_row / buf_y0 / buf_y1（随旋转 cb 而异）
  u8g2->cb->update_page_win(u8g2);	// 重算 user_x0..y1（绘制窗口）并应用 clip window
}

/**
 * @brief 分页绘制循环的"起始"：清缓冲并把页指针拨回第 0 行
 * @param  u8g2  上下文
 * @note   用法：u8g2_FirstPage(&u8g2); do { 画图 } while ( u8g2_NextPage(&u8g2) );
 *         全缓冲模式下循环体只执行一轮（tile_buf_height == tile_height），
 *         费用仅是多一次清屏+发送语义；小 RAM 页模式则每轮画一页。
 *         循环体内所有 DrawXxx 都必须每次重新执行（不能画一半留着下一轮）。
 */
void u8g2_FirstPage(u8g2_t *u8g2)
{
  if ( u8g2->is_auto_page_clear )	// u8g2_SetAutoPageClear 可关掉，追求速度时自行管理
  {
    u8g2_ClearBuffer(u8g2);		// 每页都从头清一次，保证页与页之间不残留上一页内容
  }
  u8g2_SetBufferCurrTileRow(u8g2, 0);	// 页指针归零，同时刷新裁剪窗口
}

/**
 * @brief 分页绘制循环的"推进"：把当前缓冲发往显示 RAM，再翻到下一页
 * @return 1=还有下一页（继续循环体），0=整屏发送完成（退出循环）
 * @note   顺序很重要：先发（把刚画好的本页落进显示 RAM），再判断、清屏、翻页。
 *         最后一页发完即触发 RefreshDisplay（电子纸在此才真正成像）。
 *         注意循环体内绘制调用的次数在各页间必须相同，否则页间会闪烁/错位。
 */
uint8_t u8g2_NextPage(u8g2_t *u8g2)
{
  uint8_t row;
  u8g2_send_buffer(u8g2);		// 当前页内容 → 显示 RAM（从 tile_curr_row 起共 tile_buf_height 行）
  row = u8g2->tile_curr_row;
  row += u8g2->tile_buf_height;		// 页指针前移一个缓冲高度
  if ( row >= u8g2_GetU8x8(u8g2)->display_info->tile_height )
  {
    u8x8_RefreshDisplay( u8g2_GetU8x8(u8g2) );	// 所有页发完：刷新（电子纸成像；其他屏多为空操作）
    return 0;				// 绘制循环结束
  }
  if ( u8g2->is_auto_page_clear )
  {
    u8g2_ClearBuffer(u8g2);		// 下一页从干净的缓冲开始
  }
  u8g2_SetBufferCurrTileRow(u8g2, row);	// 翻页并重算裁剪窗口——下一轮画图落在新的页窗口内
  return 1;				// 还有页没画，继续循环
}



/*============================================*/
/*
  Description:
    Update a sub area of the display, given by tile position, width and height.
    The arguments are "tile" coordinates. Any u8g2 rotation is ignored.
    This procedure only checks whether full buffer mode is active.
    There is no error checking for the arguments: It is the responsibility of the
    user to ensure, that the provided arguments are correct.

  Limitations:
    - Only available in full buffer mode (will not do anything in page mode)
    - Tile positions and sizes (pixel position divided by 8)
    - Any display rotation/mirror is ignored
    - Only works with displays, which support U8x8 API
    - Will not send the e-paper refresh message (will probably not work with e-paper devices)
*/
void u8g2_UpdateDisplayArea(u8g2_t *u8g2, uint8_t  tx, uint8_t ty, uint8_t tw, uint8_t th)
{
  uint16_t page_size;
  uint8_t *ptr;

  /* check, whether we are in full buffer mode */
  if ( u8g2->tile_buf_height != u8g2_GetU8x8(u8g2)->display_info->tile_height )
    return; /* not in full buffer mode, do nothing */	// 页模式缓冲与屏不同高，区域坐标对不上，直接放弃

  page_size = u8g2->pixel_buf_width;  /* 8*u8g2->u8g2_GetU8x8(u8g2)->display_info->tile_width */

  ptr = u8g2_GetBufferPtr(u8g2);	// 从缓冲 (tx,ty) 处开始：tx 单位是 tile，换算成字节要 ×8
  ptr += tx*8;
  ptr += page_size*ty;			// ty 行的偏移 = ty × 每行字节数

  while( th > 0 )
  {
    u8x8_DrawTile( u8g2_GetU8x8(u8g2), tx, ty, tw, ptr );	// 一次发该 tile 行内的 tw 个 tile
    ptr += page_size;		// 前进到缓冲的下一 tile 行
    ty++;			// 显示 RAM 目标行也下移一行
    th--;			// 还有 th 行要发
  }
}

/* same as sendBuffer, but does not send the ePaper refresh message */
/**
 * @brief 把整块缓冲写入显示 RAM，但不发刷新消息（SendBuffer 的"静音版"）
 * @param  u8g2  上下文
 * @note   与 u8g2_SendBuffer 的唯一区别是末尾少一个 u8x8_RefreshDisplay：
 *         普通 OLED/LCD 两者几乎等价；电子纸屏则"只改 RAM 不成像"，
 *         适合先把画面准备好、等合适时机再统一刷新的场景。
 *         全缓冲模式 + 频繁局部更新时可配合 UpdateDisplayArea 降低总线流量。
 */
void u8g2_UpdateDisplay(u8g2_t *u8g2)
{
  u8g2_send_buffer(u8g2);	// 复用内部整帧搬运循环（tile_curr_row 通常已归零）
}


/*============================================*/

/**
 * @brief 把缓冲导出为 PBM 文本流（"vertical_top" 竖排字节架构），经 out 回调逐段输出
 * @param  out  字符串输出回调（如打印到串口/文件）
 * @note   调试用：get_pixel_1 按"每字节竖排 8 像素（LSB 在上）"读位，
 *         与 SSD1306 类控制器的显存排布一致，导出图可直接肉眼比对缓冲内容。
 */
/* vertical_top memory architecture */
void u8g2_WriteBufferPBM(u8g2_t *u8g2, void (*out)(const char *s))
{
  u8x8_capture_write_pbm_pre(u8g2_GetBufferTileWidth(u8g2), u8g2_GetBufferTileHeight(u8g2), out);
  u8x8_capture_write_pbm_buffer(u8g2_GetBufferPtr(u8g2), u8g2_GetBufferTileWidth(u8g2), u8g2_GetBufferTileHeight(u8g2), u8x8_capture_get_pixel_1, out);
}

/**
 * @brief 把缓冲导出为 XBM 格式文本流（竖排字节架构），常用于写回 C 数组做截图比对
 * @param  out  字符串输出回调
 */
void u8g2_WriteBufferXBM(u8g2_t *u8g2, void (*out)(const char *s))
{
  u8x8_capture_write_xbm_pre(u8g2_GetBufferTileWidth(u8g2), u8g2_GetBufferTileHeight(u8g2), out);
  u8x8_capture_write_xbm_buffer(u8g2_GetBufferPtr(u8g2), u8g2_GetBufferTileWidth(u8g2), u8g2_GetBufferTileHeight(u8g2), u8x8_capture_get_pixel_1, out);
}


/* horizontal right memory architecture */
/* SH1122, LD7032, ST7920, ST7986, LC7981, T6963, SED1330, RA8835, MAX7219, LS0 */
/**
 * @brief PBM 导出的"横排字节"变体：每字节横排 8 像素（LSB 在左）
 * @param  out  字符串输出回调
 * @note   横排架构屏（见上方注释的控制器列表）的缓冲位序与竖排不同，
 *         必须用 get_pixel_2 读取才能得到正确图像。
 */
void u8g2_WriteBufferPBM2(u8g2_t *u8g2, void (*out)(const char *s))
{
  u8x8_capture_write_pbm_pre(u8g2_GetBufferTileWidth(u8g2), u8g2_GetBufferTileHeight(u8g2), out);
  u8x8_capture_write_pbm_buffer(u8g2_GetBufferPtr(u8g2), u8g2_GetBufferTileWidth(u8g2), u8g2_GetBufferTileHeight(u8g2), u8x8_capture_get_pixel_2, out);
}

/**
 * @brief XBM 导出的"横排字节"变体，配套 PBM2 使用
 * @param  out  字符串输出回调
 */
void u8g2_WriteBufferXBM2(u8g2_t *u8g2, void (*out)(const char *s))
{
  u8x8_capture_write_xbm_pre(u8g2_GetBufferTileWidth(u8g2), u8g2_GetBufferTileHeight(u8g2), out);
  u8x8_capture_write_xbm_buffer(u8g2_GetBufferPtr(u8g2), u8g2_GetBufferTileWidth(u8g2), u8g2_GetBufferTileHeight(u8g2), u8x8_capture_get_pixel_2, out);
}

