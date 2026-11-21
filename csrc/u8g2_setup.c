/*
 * 【中文导读】u8g2_setup.c —— 显示器初始化总装（A级逐函数深注）
 * u8g2_SetupDisplay/SetupFonts：把"屏幕驱动回调+显示尺寸+字体"装进 u8g2_t。
 */

/*

  u8g2_setup.c

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
#include <assert.h>


/*============================================*/


#ifdef U8G2_WITH_CLIP_WINDOW_SUPPORT

/**
 * @brief 把用户裁剪窗口恢复为"无限大"（等效关闭裁剪）
 * @param  u8g2  上下文
 * @note   x1/y1 用 ~0（uint16 下即 65535）当哨兵值，比写成屏宽/屏高更省事：
 *         后续与 user 窗口求交时结果自然等于 user 窗口本身。
 *         末尾必须回调 update_page_win 让新窗口立即生效。
 */
void u8g2_SetMaxClipWindow(u8g2_t *u8g2)
{
  u8g2->clip_x0 = 0;
  u8g2->clip_y0 = 0;
  u8g2->clip_x1 = (u8g2_uint_t)~(u8g2_uint_t)0;	// 取反 0 = 类型最大值，作为"无上界"哨兵
  u8g2->clip_y1 = (u8g2_uint_t)~(u8g2_uint_t)0;

  u8g2->cb->update_page_win(u8g2);		// 重算 user 窗口（user ∩ clip）
}

/**
 * @brief 设置用户裁剪窗口（x1/y1 为开区间右/下界，不含该像素）
 * @param  clip_x0/clip_y0  窗口左上角（含）
 * @param  clip_x1/clip_y1  窗口右下角（不含）
 * @note   之后所有 DrawXxx 都画不出窗口外；窗口与当前页无交叠时
 *         is_page_clip_window_intersection=0，绘制会被提前短路（省 CPU）。
 */
void u8g2_SetClipWindow(u8g2_t *u8g2, u8g2_uint_t clip_x0, u8g2_uint_t clip_y0, u8g2_uint_t clip_x1, u8g2_uint_t clip_y1 )
{
  u8g2->clip_x0 = clip_x0;	// 直接保存用户给的值，不与当前窗口求交——
  u8g2->clip_y0 = clip_y0;	// 交集推迟到 update_page_win 里做，这样可反复改窗口
  u8g2->clip_x1 = clip_x1;
  u8g2->clip_y1 = clip_y1;
  u8g2->cb->update_page_win(u8g2);	// 立即生效：user 窗口 = 页窗口 ∩ clip 窗口
}
#endif

/*============================================*/
/*
  This procedure is called after setting up the display (u8x8 structure).
  --> This is the central init procedure for u8g2 object
*/
/**
 * @brief u8g2 对象的中央初始化：装缓冲、装像素序回调、装旋转回调，并算出初始裁剪窗口
 * @param  buf               帧缓冲首地址（全缓冲 = 满屏大小；页模式只有 1~2 个 tile 行）
 * @param  tile_buf_height   缓冲高度（tile 行数）：_f 满屏 / _2 两页 / _1 单页
 * @param  ll_hvline_cb      底层 hvline 像素写入函数（按控制器显存位序二选一）
 * @param  u8g2_cb           旋转回调组 U8G2_R0..R3 / MIRROR
 * @note   所有 u8g2_Setup_xxx_1/2/f() 的公共终点：d_setup 表只负责"选驱动+选缓冲"，
 *         真正的字段装配都在这里。调用前提：u8g2_SetupDisplay 已先装好 u8x8 回调链。
 */
void u8g2_SetupBuffer(u8g2_t *u8g2, uint8_t *buf, uint8_t tile_buf_height, u8g2_draw_ll_hvline_cb ll_hvline_cb, const u8g2_cb_t *u8g2_cb)
{
  u8g2->font = NULL;			// 字体必须等用户 u8g2_SetFont() 再给，画字前未设置是常见 crash 点
  //u8g2->kerning = NULL;
  //u8g2->get_kerning_cb = u8g2_GetNullKerning;

  //u8g2->ll_hvline = u8g2_ll_hvline_vertical_top_lsb;
  u8g2->ll_hvline = ll_hvline_cb;	// 由 setup 宏按控制器显存位序选：竖排(top_lsb) 或 横排(right_lsb)

  u8g2->tile_buf_ptr = buf;		// 帧缓冲首地址（来自 u8g2_d_memory.c 的静态数组）
  u8g2->tile_buf_height = tile_buf_height;	// 缓冲 tile 行数：决定 _1/_2/_f 三种内存模式的本质

  u8g2->tile_curr_row = 0;		// 页指针归零（全缓冲后一直是 0）

  u8g2->font_decode.is_transparent = 0; /* issue 443 */	// 字体默认"实心"模式：背景像素也要画
  u8g2->bitmap_transparency = 0;	// 位图同理，默认把 0 位当黑画出来

  u8g2->font_height_mode = 0; /* issue 2046 */	// 字高参考模式默认 TEXT
  u8g2->draw_color = 1;			// 默认画"白"(点亮像素)，0 为擦除，可被 SetDrawColor 改
  u8g2->is_auto_page_clear = 1;		// FirstPage/NextPage 默认自动清缓冲

  u8g2->cb = u8g2_cb;			// 装 R0..R3/mirror 旋转回调组
  u8g2->cb->update_dimension(u8g2);	// 由缓冲高度+页行号推 pixel/buf 尺寸
#ifdef U8G2_WITH_CLIP_WINDOW_SUPPORT
  u8g2_SetMaxClipWindow(u8g2);		/* assign a clip window and call the update() procedure */	// 初始 clip=无限大
#else
  u8g2->cb->update_page_win(u8g2);
#endif

  u8g2_SetFontPosBaseline(u8g2);  /* issue 195 */	// 文字定位基准默认"基线"，避免早期版本行为差异

#ifdef U8G2_WITH_FONT_ROTATION
  u8g2->font_decode.dir = 0;		// 字体绘制方向默认 0（水平向右）
#endif
}

/*
  Usually the display rotation is set initially, but it could be done later also
  u8g2_cb can be U8G2_R0..U8G2_R3
*/
/**
 * @brief 运行期切换旋转/镜像模式（u8g2_begin 之后任何时刻可调）
 * @param  u8g2_cb  U8G2_R0/R1/R2/R3/MIRROR/MIRROR_VERTICAL
 * @note   只换 u8g2->cb 三件套并重算窗口，缓冲内容不搬动——
 *         切换后旧画面在新坐标系下不会自动转过来，一般换旋转后紧跟 Clear+重画。
 *         注意宽高互换发生在 update_dimension_r1/r3 里（width/height 对调）。
 */
void u8g2_SetDisplayRotation(u8g2_t *u8g2, const u8g2_cb_t *u8g2_cb)
{
  u8g2->cb = u8g2_cb;			// 换回调组：后续所有 draw/翻页都经它变形
  u8g2->cb->update_dimension(u8g2);	// 重算 width/height（R1/R3 会交换横竖）
  u8g2->cb->update_page_win(u8g2);	// 重算本页的用户窗口
}

/*============================================*/

/**
 * @brief 直接向显示控制器发一条 printf 风格的命令/数据序列（绕过缓冲）
 * @param  fmt  格式串，配 u8x8_cad_vsendf 的转义语法（如 'A'后跟参数字节等）
 * @note   透传通道：不经过 tile 缓冲、不做裁剪，直接落到 cad→byte 层，
 *         用于控制器专属命令（例如自定义 ST7920 反白、设置扫描方向）。
 *         参数包可变，用 va_list 展开后逐字节交给 cad 层。
 */
void u8g2_SendF(u8g2_t * u8g2, const char *fmt, ...)
{
  va_list va;
  va_start(va, fmt);
  u8x8_cad_vsendf(u8g2_GetU8x8(u8g2), fmt, va);	// 格式串驱动：cmd/arg/data 混合发送
  va_end(va);
}


/*============================================*/
/*
  update dimension:
  calculate the following variables:
    u8g2_uint_t buf_x0;	left corner of the buffer
    u8g2_uint_t buf_x1;	right corner of the buffer (excluded)
    u8g2_uint_t buf_y0;
    u8g2_uint_t buf_y1;
*/
/**
 * @brief 旋转无关的公共尺寸计算：缓冲像素尺寸 + 本页纵向区间 [buf_y0, buf_y1) + 用户可见宽高
 * @param  u8g2  上下文（依赖 tile_buf_height / tile_curr_row / display_info）
 * @note   被 R0..R3 四套 update_dimension 回调共同调用，是 hvline 纵向裁剪的数据来源。
 *         pixel_curr_row 是"当前页首行像素 y"——页模式下它随翻页前移，
 *         从而把绘制窗口推向屏幕下方的未显示区域。
 */

static void u8g2_update_dimension_common(u8g2_t *u8g2)
{
  const u8x8_display_info_t *display_info = u8g2_GetU8x8(u8g2)->display_info;
  u8g2_uint_t t;

  t = u8g2->tile_buf_height;
  t *= 8;
  u8g2->pixel_buf_height = t;		// 缓冲高度换算成像素（tile 行数 × 8）

  t = display_info->tile_width;
#ifndef U8G2_16BIT
  if ( t >= 32 )
    t = 31;				// 8 位坐标模式：宽度封顶 248 像素，防 u8g2_uint_t(8bit) 溢出
#endif
  t *= 8;
  u8g2->pixel_buf_width = t;		// 缓冲宽度像素（tile_width × 8）

  t = u8g2->tile_curr_row;
  t *= 8;
  u8g2->pixel_curr_row = t;		// 当前页首行对应的像素 y（页模式的窗口基点）

  t = u8g2->tile_buf_height;
  /* handle the case, where the buffer is larger than the (remaining) part of the display */
  if ( t + u8g2->tile_curr_row > display_info->tile_height )
    t = display_info->tile_height - u8g2->tile_curr_row;	// 最后几页：有效行数不足一个缓冲，截断
  t *= 8;

  u8g2->buf_y0 = u8g2->pixel_curr_row;	// 本页缓冲覆盖的纵向区间 [buf_y0, buf_y1)
  u8g2->buf_y1 = u8g2->buf_y0;
  u8g2->buf_y1 += t;			// hvline 裁剪靠它：y 落在区间外直接丢弃


#ifdef U8G2_16BIT
  u8g2->width = display_info->pixel_width;	// 16 位模式：直接用驱动表里的真实像素宽高
  u8g2->height = display_info->pixel_height;	// （允许 128x32 这类"tile 高 4 行、像素高 32"的精确值）
#else
  u8g2->width = 240;			// 8 位模式给宽度设上限 240（坐标只有 0..255，留出余量）
  if ( display_info->pixel_width <= 240 )
    u8g2->width = display_info->pixel_width;
  u8g2->height = display_info->pixel_height;
#endif

}

/*==========================================================*/
/* apply clip window */

#ifdef U8G2_WITH_CLIP_WINDOW_SUPPORT
/**
 * @brief 把用户裁剪窗口套到当前页窗口上：user = user ∩ clip，并记录"是否有交叠"
 * @param  u8g2  上下文
 * @note   交叠标志 is_page_clip_window_intersection 是页模式的速度关键：
 *         为 0 时画图函数（含字体）直接跳过本页所有绘制，省掉整页无效计算。
 *         原地收缩 user_x0..y1（四组 min/max），后续 IsIntersection/DrawingHVLine
 *         都以收缩后的值为准。
 */
static void u8g2_apply_clip_window(u8g2_t *u8g2)
{
  /* check aganst the current user_??? window */
  if ( u8g2_IsIntersection(u8g2, u8g2->clip_x0, u8g2->clip_y0, u8g2->clip_x1, u8g2->clip_y1) == 0 )
  {
    u8g2->is_page_clip_window_intersection = 0;	// clip 窗口与当前页完全不相交：本页不用画
  }
  else
  {
    u8g2->is_page_clip_window_intersection = 1;	// 有交叠，允许绘制（但只画交集区域）

    if ( u8g2->user_x0 < u8g2->clip_x0 )
      u8g2->user_x0 = u8g2->clip_x0;		// 四组收缩：user 窗口被 clip 窗口裁小
    if ( u8g2->user_x1 > u8g2->clip_x1 )
      u8g2->user_x1 = u8g2->clip_x1;
    if ( u8g2->user_y0 < u8g2->clip_y0 )
      u8g2->user_y0 = u8g2->clip_y0;
    if ( u8g2->user_y1 > u8g2->clip_y1 )
      u8g2->user_y1 = u8g2->clip_y1;
  }
}
#endif /* U8G2_WITH_CLIP_WINDOW_SUPPORT */

/*==========================================================*/


/**
 * @brief R0（不旋转）的尺寸更新：直接用公共计算
 * @note   R0/R2 共用 common 结果；R1/R3 还要在此之上交换 width/height。
 */
void u8g2_update_dimension_r0(u8g2_t *u8g2)
{
  u8g2_update_dimension_common(u8g2);
}

/**
 * @brief R0 的页窗口：x 覆盖整屏宽，y 取本页缓冲区间
 * @note   user 窗口 = [0,width) × [buf_y0,buf_y1)——页模式下这就是"当前正在画的那一页"，
 *         窗口外的图元会被 hvline 裁剪掉。
 */
void u8g2_update_page_win_r0(u8g2_t *u8g2)
{
  u8g2->user_x0 = 0;
  u8g2->user_x1 = u8g2->width;			/* pixel_buf_width replaced with width */	// 用真实像素宽（可能小于 tile 宽×8）

  u8g2->user_y0 = u8g2->buf_y0;			// 页窗口随翻页下移
  u8g2->user_y1 = u8g2->buf_y1;

#ifdef U8G2_WITH_CLIP_WINDOW_SUPPORT
  u8g2_apply_clip_window(u8g2);			// 再与 clip 窗口求交
#endif /* U8G2_WITH_CLIP_WINDOW_SUPPORT */
}


/**
 * @brief R1（顺时针 90°）尺寸更新：宽高互换
 * @note   物理屏 128x64 旋转后用户看到 64x128——width 取物理高、height 取物理宽。
 */
void u8g2_update_dimension_r1(u8g2_t *u8g2)
{
  u8g2_update_dimension_common(u8g2);

  u8g2->height = u8g2_GetU8x8(u8g2)->display_info->pixel_width;	// 交换：用户坐标系的宽=物理屏高
  u8g2->width = u8g2_GetU8x8(u8g2)->display_info->pixel_height;

}

/**
 * @brief R1 的页窗口：页方向变成横向（x 跟随 buf_y0/y1），纵向覆盖交换后的 height
 * @note   旋转 90° 后缓冲的"行方向"对应屏幕的"列方向"，
 *         所以 user_x 区间来自 buf_y0/buf_y1 而不是宽度。
 */
void u8g2_update_page_win_r1(u8g2_t *u8g2)
{
  u8g2->user_x0 = u8g2->buf_y0;		// 页边界映射到 x 轴
  u8g2->user_x1 = u8g2->buf_y1;

  u8g2->user_y0 = 0;
  u8g2->user_y1 = u8g2->height;	/* pixel_buf_width replaced with height (which is the real pixel width) */

#ifdef U8G2_WITH_CLIP_WINDOW_SUPPORT
  u8g2_apply_clip_window(u8g2);
#endif /* U8G2_WITH_CLIP_WINDOW_SUPPORT */
}

/**
 * @brief R2（180°）尺寸更新：与 R0 相同
 */
void u8g2_update_dimension_r2(u8g2_t *u8g2)
{
  u8g2_update_dimension_common(u8g2);
}

/**
 * @brief R2 的页窗口：倒置坐标系——页从屏幕底部往上推进
 * @note   user_y0 = height - buf_y1、user_y1 = height - buf_y0，
 *         即把 buf 区间按中轴镜像；"height 非整 8 倍数"时 buf_y1 可能大于 height，
 *         所以先判 height >= buf_y1 防止下溢（负数回绕成巨大无符号值）。
 */
void u8g2_update_page_win_r2(u8g2_t *u8g2)
{
  u8g2->user_x0 = 0;
  u8g2->user_x1 = u8g2->width;	/* pixel_buf_width replaced with width */

  /* there are ases where the height is not a multiple of 8. */
  /* in such a case u8g2->buf_y1 might be heigher than u8g2->height */
  u8g2->user_y0 = 0;
  if ( u8g2->height >= u8g2->buf_y1 )		// 无符号减法保护：先确认够减
    u8g2->user_y0 = u8g2->height - u8g2->buf_y1;
  u8g2->user_y1 = u8g2->height - u8g2->buf_y0;

#ifdef U8G2_WITH_CLIP_WINDOW_SUPPORT
  u8g2_apply_clip_window(u8g2);
#endif /* U8G2_WITH_CLIP_WINDOW_SUPPORT */
}


/**
 * @brief R3（270°）尺寸更新：宽高互换（同 R1）
 */
void u8g2_update_dimension_r3(u8g2_t *u8g2)
{
  u8g2_update_dimension_common(u8g2);

  u8g2->height = u8g2_GetU8x8(u8g2)->display_info->pixel_width;
  u8g2->width = u8g2_GetU8x8(u8g2)->display_info->pixel_height;

}

/**
 * @brief R3 的页窗口：镜像方向与 R2 相同、作用在 x 轴（因宽高已互换）
 * @note   x 区间 = width - buf_y1 .. width - buf_y0（同样带下溢保护），
 *         y 区间覆盖交换后的 height。
 */
void u8g2_update_page_win_r3(u8g2_t *u8g2)
{
  /* there are ases where the height is not a multiple of 8. */
  /* in such a case u8g2->buf_y1 might be heigher than u8g2->width */
  u8g2->user_x0 = 0;
  if ( u8g2->width >= u8g2->buf_y1 )		// R2 的镜像逻辑搬到 x 轴
    u8g2->user_x0 = u8g2->width - u8g2->buf_y1;
  u8g2->user_x1 = u8g2->width - u8g2->buf_y0;

  u8g2->user_y0 = 0;
  u8g2->user_y1 = u8g2->height;	/* pixel_buf_width replaced with height (pixel_width) */

#ifdef U8G2_WITH_CLIP_WINDOW_SUPPORT
  u8g2_apply_clip_window(u8g2);
#endif /* U8G2_WITH_CLIP_WINDOW_SUPPORT */
}


/*============================================*/
extern void u8g2_draw_hv_line_2dir(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t len, uint8_t dir);


/**
 * @brief R0 旋转回调的绘制入口：原样透传给 hvline（唯一不变形的一档）
 * @param  dir  0=水平线（向右），1=垂直线（向下）
 * @note   draw_l90 是 cb 组的第三个回调：上层图元（框/圆/字体）把线段"归一化成
 *         起点+长度+方向"后统一走这里，再由它分发给 u8g2_draw_hv_line_2dir
 *         （后者做裁剪并调用 ll_hvline 写像素）。
 */
void u8g2_draw_l90_r0(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t len, uint8_t dir)
{
#ifdef __unix
  assert( dir <= 1 );
#endif
  u8g2_draw_hv_line_2dir(u8g2, x, y, len, dir);
}

/**
 * @brief 水平镜像绘制（U8G2_MIRROR）：x 以屏幕中轴左右翻转
 * @note   (dir&1)==0 是水平线：镜像后起点变成 width-x-len；
 *         垂直线只翻 x 坐标本身，取 width-x-1。y 方向不动。
 */
void u8g2_draw_l90_mirrorr_r0(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t len, uint8_t dir)
{
  u8g2_uint_t xx;
  xx = u8g2->width;
  xx -= x;
  if ( (dir & 1) == 0 )		// 水平线：线段整体翻转，起点平移一个长度
  {
    xx -= len;
  }
  else
  {
    xx--;			// 垂直线：单像素列，减 1 对齐到镜像位置
  }
  u8g2_draw_hv_line_2dir(u8g2, xx, y, len, dir);
}

/**
 * @brief 垂直镜像绘制（U8G2_MIRROR_VERTICAL）：y 以屏幕中轴上下翻转
 * @note   与水平镜像对称：垂直线(dir&1==1)整体翻转平移 len，水平线只翻 y-1。
 */
void u8g2_draw_mirror_vertical_r0(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t len, uint8_t dir)
{
  u8g2_uint_t yy;
  yy = u8g2->height;
  yy -= y;
  if ( (dir & 1) == 1 )		// 垂直线：整体翻转
  {
    yy -= len;
  }
  else
  {
    yy--;			// 水平线：单像素行
  }
  u8g2_draw_hv_line_2dir(u8g2, x, yy, len, dir);
}

/* dir = 0 or 1 */
/**
 * @brief R1（顺时针 90°）绘制变形：用户坐标 (x,y) → 物理 (height-1-y, x)
 * @note   核心技巧：dir 同步 +1 取模 2——水平线转完变垂直线。
 *         代码用 dir++ 后判断是否回到 0 实现"取模"，并顺手把线段起点
 *         平移 len（xx -= len; xx++），避免分支重复计算。
 */
void u8g2_draw_l90_r1(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t len, uint8_t dir)
{
  u8g2_uint_t xx, yy;

#ifdef __unix
  assert( dir <= 1 );
#endif

  yy = x;			// 旋转后纵坐标 = 原横坐标

  xx = u8g2->height;
  xx -= y;
  xx--;				// 旋转后横坐标 = height-1-y（绕屏幕左上角转）

  dir ++;			// 方向跟着转 90°：0(横)→1(竖)，1(竖)→0(横)
  if ( dir == 2 )
  {
    xx -= len;			// 转回水平方向的线：起点左移线长（方向反转补偿）
    xx++;
    dir = 0;
  }

  u8g2_draw_hv_line_2dir(u8g2, xx, yy, len, dir);
}

/**
 * @brief R2（180°）绘制变形：用户 (x,y) → 物理 (width-x, height-y) 再按方向微调
 * @note   坐标先减不退 1，分支里再补 --；水平线起点左移 len、垂直线起点上移 len。
 *         上方被注释掉的旧实现是等价写法，保留作历史参考（勿删）。
 */
void u8g2_draw_l90_r2(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t len, uint8_t dir)
{
  u8g2_uint_t xx, yy;

  /*
  yy = u8g2->height;
  yy -= y;
  yy--;

  xx = u8g2->width;
  xx -= x;
  xx--;

  if ( dir == 0 )
  {
    xx -= len;
    xx++;
  }
  else if ( dir == 1 )
  {
    yy -= len;
    yy++;
  }
  */

  yy = u8g2->height;
  yy -= y;			// 上下翻转

  xx = u8g2->width;
  xx -= x;			// 左右翻转

  if ( dir == 0 )
  {
    yy--;			// 水平线：终点对齐后起点在 xx-len
    xx -= len;
  }
  else if ( dir == 1 )
  {
    xx--;			// 垂直线：起点上移 len
    yy -= len;
  }

  u8g2_draw_hv_line_2dir(u8g2, xx, yy, len, dir);
}

/**
 * @brief R3（270°）绘制变形：用户 (x,y) → 物理 (y, width-1-x)，dir 同步反转
 * @note   与 R1 镜像对称：水平线转成垂直线时同样把 len 计入起点（yy -= len; yy++ 的展开）。
 */
void u8g2_draw_l90_r3(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t len, uint8_t dir)
{
  u8g2_uint_t xx, yy;

  xx = y;			// 旋转后横坐标 = 原纵坐标

  yy = u8g2->width;
  yy -= x;			// 旋转后纵坐标 = width-1-x

  if ( dir == 0 )
  {
    yy--;			// 水平线转垂直线：起点 = width-1-x-len（三步展开）
    yy -= len;
    yy++;
    dir = 1;
  }
  else
  {
    yy--;			// 垂直线转水平线
    dir = 0;
  }


  u8g2_draw_hv_line_2dir(u8g2, xx, yy, len, dir);
}



/*============================================*/
/**
 * @brief 6 组旋转/镜像回调常量表：u8g2_Setup_xxx(rotation) 的 rotation 实参就指向这里
 * @note   u8g2_cb_t 只含三个函数指针（update_dimension / update_page_win / draw_l90），
 *         旋转 = 换一组函数指针，零运行时开销；R1/R3 复用 common 尺寸计算再交换宽高，
 *         mirror 复用 R0 的窗口逻辑只换 draw 回调。
 */
const u8g2_cb_t u8g2_cb_r0 = { u8g2_update_dimension_r0, u8g2_update_page_win_r0, u8g2_draw_l90_r0 };
const u8g2_cb_t u8g2_cb_r1 = { u8g2_update_dimension_r1, u8g2_update_page_win_r1, u8g2_draw_l90_r1 };
const u8g2_cb_t u8g2_cb_r2 = { u8g2_update_dimension_r2, u8g2_update_page_win_r2, u8g2_draw_l90_r2 };
const u8g2_cb_t u8g2_cb_r3 = { u8g2_update_dimension_r3, u8g2_update_page_win_r3, u8g2_draw_l90_r3 };

const u8g2_cb_t u8g2_cb_mirror = { u8g2_update_dimension_r0, u8g2_update_page_win_r0, u8g2_draw_l90_mirrorr_r0 };
const u8g2_cb_t u8g2_cb_mirror_vertical = { u8g2_update_dimension_r0, u8g2_update_page_win_r0, u8g2_draw_mirror_vertical_r0 };

/*============================================*/
/* setup for the null device */

/* setup for the null (empty) device */
/**
 * @brief 空设备装配：所有绘制进 8 字节假缓冲，通信层全部空操作
 * @param  byte_cb / gpio_and_delay_cb  用户仍需提供（可为 u8x8_byte_empty / dummy）
 * @note   用途：无屏环境下跑图形逻辑、CI 冒烟测试。buf 只有 1 个 tile 行静态数组，
 *         display 回调是 u8x8_d_null_cb，cad 是 u8x8_cad_empty——任何写入都被安全吞掉。
 */
void u8g2_Setup_null(u8g2_t *u8g2, const u8g2_cb_t *rotation, u8x8_msg_cb byte_cb, u8x8_msg_cb gpio_and_delay_cb)
{
  static uint8_t buf[8];	// 仅 1 个 tile（8 字节）——最小可能的"屏"
  u8g2_SetupDisplay(u8g2, u8x8_d_null_cb, u8x8_cad_empty, byte_cb, gpio_and_delay_cb);
  u8g2_SetupBuffer(u8g2, buf, 1, u8g2_ll_hvline_vertical_top_lsb, rotation);
}


