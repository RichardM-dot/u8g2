/*
 * ============================================================================
 * 【中文导读】u8g2_font.c —— 字体渲染引擎
 * ----------------------------------------------------------------------------
 * u8g2 自带字体是"游程编码(RLE)位图"格式，本文件解码渲染：
 * u8g2_font_get_byte/glyph 控制字形数据的读取，
 * u8g2_font_draw_glyph 逐像素还原，支持透明/实心两种模式。
 * 读它可以搞懂"字库是怎么在单片机上渲染出来的"。
 * ============================================================================
 */

/*

  u8g2_font.c

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

/* size of the font data structure, there is no struct or class... */
/* this is the size for the new font format */
#define U8G2_FONT_DATA_STRUCT_SIZE 23

/*
  font data:

  offset	bytes	description
  0		1		glyph_cnt		number of glyphs
  1		1		bbx_mode	0: proportional, 1: common height, 2: monospace, 3: multiple of 8
  2		1		bits_per_0	glyph rle parameter
  3		1		bits_per_1	glyph rle parameter

  4		1		bits_per_char_width		glyph rle parameter
  5		1		bits_per_char_height	glyph rle parameter
  6		1		bits_per_char_x		glyph rle parameter
  7		1		bits_per_char_y		glyph rle parameter
  8		1		bits_per_delta_x		glyph rle parameter

  9		1		max_char_width
  10		1		max_char_height
  11		1		x offset
  12		1		y offset (descent)
  
  13		1		ascent (capital A)
  14		1		descent (lower g)
  15		1		ascent '('
  16		1		descent ')'
  
  17		1		start pos 'A' high byte
  18		1		start pos 'A' low byte

  19		1		start pos 'a' high byte
  20		1		start pos 'a' low byte

  21		1		start pos unicode high byte
  22		1		start pos unicode low byte

  Font build mode, 0: proportional, 1: common height, 2: monospace, 3: multiple of 8

  Font build mode 0:		
    - "t"
    - Ref height mode: U8G2_FONT_HEIGHT_MODE_TEXT, U8G2_FONT_HEIGHT_MODE_XTEXT or U8G2_FONT_HEIGHT_MODE_ALL
    - use in transparent mode only (does not look good in solid mode)
    - most compact format
    - different font heights possible
    
  Font build mode 1:		
    - "h"
    - Ref height mode: U8G2_FONT_HEIGHT_MODE_ALL
    - transparent or solid mode
    - The height of the glyphs depend on the largest glyph in the font. This means font height depends on postfix "r", "f" and "n".

*/

/* use case: What is the width and the height of the minimal box into which string s fints? */
void u8g2_font_GetStrSize(const void *font, const char *s, u8g2_uint_t *width, u8g2_uint_t *height);
void u8g2_font_GetStrSizeP(const void *font, const char *s, u8g2_uint_t *width, u8g2_uint_t *height);

/* use case: lower left edge of a minimal box is known, what is the correct x, y position for the string draw procedure */
void u8g2_font_AdjustXYToDraw(const void *font, const char *s, u8g2_uint_t *x, u8g2_uint_t *y);
void u8g2_font_AdjustXYToDrawP(const void *font, const char *s, u8g2_uint_t *x, u8g2_uint_t *y);

/* use case: Baseline origin known, return minimal box */
void u8g2_font_GetStrMinBox(u8g2_t *u8g2, const void *font, const char *s, u8g2_uint_t *x, u8g2_uint_t *y, u8g2_uint_t *width, u8g2_uint_t *height);

/* procedures */

/*========================================================================*/
/* low level byte and word access */

/* removed NOINLINE, because it leads to smaller code, might also be faster */
/**
 * @brief   读取字体数据中偏移 offset 处的 1 字节
 * @param   font    字体数组基址（可能位于 AVR 的 Flash/PROGMEM 空间）
 * @param   offset  字节偏移 0~255（仅用于 23 字节字体头内寻址）
 * @return  指定偏移处的字节值
 * @note    所有字体访问都经 u8x8_pgm_read 包一层：普通 MCU 等价于 *p，
 *          AVR 上则走 LPM 指令从 Flash 读——这是"字体常量放 Flash"的关键适配点
 */
//static uint8_t u8g2_font_get_byte(const uint8_t *font, uint8_t offset) U8G2_NOINLINE;
static uint8_t u8g2_font_get_byte(const uint8_t *font, uint8_t offset)
{
  font += offset;
  return u8x8_pgm_read( font );  
}

/**
 * @brief   读取字体数据中偏移 offset 处的 16 位字（大端：高字节在前）
 * @param   font    字体数组基址
 * @param   offset  字节偏移（17/19/21，对应 start_pos_* 分段跳转指针）
 * @return  16 位无符号值 = (font[offset] << 8) | font[offset+1]
 * @note    u8g2 字体内部固定大端序，与目标 MCU 的内存字节序无关，由本函数手工拼装
 */
static uint16_t u8g2_font_get_word(const uint8_t *font, uint8_t offset) U8G2_NOINLINE; 
static uint16_t u8g2_font_get_word(const uint8_t *font, uint8_t offset)
{
    uint16_t pos;
    font += offset;
    pos = u8x8_pgm_read( font );
    font++;
    pos <<= 8;
    pos += u8x8_pgm_read( font);
    return pos;
}

/*========================================================================*/
/* new font format */
/**
 * @brief   解析 23 字节字体头到 font_info 结构体（换字体时的"读文件头"动作）
 * @param   font_info  输出：字体元信息（RLE 位宽参数、分段起点、上下沿高度等）
 * @param   font       字体数组首地址（offset 0 即 glyph_cnt）
 * @note    u8g2_SetFont() 换字体时调用一次，之后所有 glyph 查找/解码都查 RAM 里的
 *          font_info 缓存，避免每次绘制都重复读 Flash 头；字段与 offset 的对应关系
 *          见文件头部 53~102 行的字体格式大注释
 */
void u8g2_read_font_info(u8g2_font_info_t *font_info, const uint8_t *font)
{
  /* offset 0 */
  font_info->glyph_cnt = u8g2_font_get_byte(font, 0);
  font_info->bbx_mode = u8g2_font_get_byte(font, 1);
  font_info->bits_per_0 = u8g2_font_get_byte(font, 2);
  font_info->bits_per_1 = u8g2_font_get_byte(font, 3);
  
  /* offset 4 */
  font_info->bits_per_char_width = u8g2_font_get_byte(font, 4);
  font_info->bits_per_char_height = u8g2_font_get_byte(font, 5);
  font_info->bits_per_char_x = u8g2_font_get_byte(font, 6);
  font_info->bits_per_char_y = u8g2_font_get_byte(font, 7);
  font_info->bits_per_delta_x = u8g2_font_get_byte(font, 8);
  
  /* offset 9 */
  font_info->max_char_width = u8g2_font_get_byte(font, 9);
  font_info->max_char_height = u8g2_font_get_byte(font, 10);
  font_info->x_offset = u8g2_font_get_byte(font, 11);
  font_info->y_offset = u8g2_font_get_byte(font, 12);
  
  /* offset 13 */
  font_info->ascent_A = u8g2_font_get_byte(font, 13);
  font_info->descent_g = u8g2_font_get_byte(font, 14);
  font_info->ascent_para = u8g2_font_get_byte(font, 15);
  font_info->descent_para = u8g2_font_get_byte(font, 16);
  
  /* offset 17 */
  /* start_pos_upper_A / start_pos_lower_a 是 ASCII 区两个"分段跳转点"：
     glyph 查找从 'A' 或 'a' 段起点开始线性扫，最多扫一段而非整个字库 */
  font_info->start_pos_upper_A = u8g2_font_get_word(font, 17);
  font_info->start_pos_lower_a = u8g2_font_get_word(font, 19); 
  
  /* offset 21 */
#ifdef U8G2_WITH_UNICODE
  font_info->start_pos_unicode = u8g2_font_get_word(font, 21); 
#endif
}


/* calculate the overall length of the font, only used to create the picture for the google wiki */
/**
 * @brief   计算整个字体数组的总字节数（生成 wiki 字体布局图/调试用）
 * @param   font_arg  字体数组首地址
 * @return  字体总长度（字节）
 * @note    走法：ASCII glyph 链（每项第 2 字节 = 到下一项的距离，第 2 字节为 0 结束）
 *          → 跳过 unicode 查找表 → unicode glyph 链（每项第 3 字节 = 距离，大端 0 结束）。
 *          与 u8g2_font_get_glyph_data 的遍历方式互为印证，可对照阅读
 */
size_t u8g2_GetFontSize(const uint8_t *font_arg)
{
  uint16_t e;
  const uint8_t *font = font_arg;
  font += U8G2_FONT_DATA_STRUCT_SIZE;
  
  for(;;)
  {
    if ( u8x8_pgm_read( font + 1 ) == 0 )
      break;
    font += u8x8_pgm_read( font + 1 );
  }
  
  /* continue with unicode section */
  font += 2;

  /* skip unicode lookup table */
  font += u8g2_font_get_word(font, 0);
  
  for(;;)
  {
    e = u8x8_pgm_read( font );
    e <<= 8;
    e |= u8x8_pgm_read( font + 1 );
    if ( e == 0 )
      break;
    font += u8x8_pgm_read( font + 2 );    
  }
  
  return (font - font_arg) + 2;
}

/*========================================================================*/
/* u8g2 interface, font access */

/**
 * @brief   取字体最大字符宽度 max_char_width（字体头 offset 9）
 * @param   u8g2  主对象（读取已缓存到 RAM 的 font_info）
 * @return  字体 bounding box 宽度（像素）
 * @note    等宽字体下所有字符同宽；比例（proportional）字体下这只是"最宽字形"的宽度
 */
uint8_t u8g2_GetFontBBXWidth(u8g2_t *u8g2)
{
  return u8g2->font_info.max_char_width;		/* new font info structure */
}

/**
 * @brief   取字体最大字符高度 max_char_height（字体头 offset 10）
 * @param   u8g2  主对象
 * @return  字体 bounding box 高度（像素）
 * @note    注意：这不是 ascent！ascent = max_char_height + y_offset（见 u8g2.h 字段注释）
 */
uint8_t u8g2_GetFontBBXHeight(u8g2_t *u8g2)
{
  return u8g2->font_info.max_char_height;		/* new font info structure */
}

/**
 * @brief   取字体全局 X 偏移 x_offset（字体头 offset 11）
 * @param   u8g2  主对象
 * @return  有符号横向偏移：字形位图相对绘制原点整体平移量
 */
int8_t u8g2_GetFontBBXOffX(u8g2_t *u8g2) U8G2_NOINLINE;
int8_t u8g2_GetFontBBXOffX(u8g2_t *u8g2)
{
  return u8g2->font_info.x_offset;		/* new font info structure */
}

/**
 * @brief   取字体全局 Y 偏移 y_offset（字体头 offset 12，通常为负）
 * @param   u8g2  主对象
 * @return  有符号纵向偏移；descent = y_offset，ascent = max_char_height + y_offset
 */
int8_t u8g2_GetFontBBXOffY(u8g2_t *u8g2) U8G2_NOINLINE;
int8_t u8g2_GetFontBBXOffY(u8g2_t *u8g2)
{
  return u8g2->font_info.y_offset;		/* new font info structure */
}

/**
 * @brief   取大写 A 的高度 ascent_A（字体头 offset 13）
 * @param   u8g2  主对象
 * @return  大写字母 A 的像素高度
 * @note    U8G2_FONT_HEIGHT_MODE_TEXT 下 font_ref_ascent 就取此值（见 u8g2_UpdateRefHeight），
 *          它决定参考点换算，是多行文本排版行距的依据
 */
uint8_t u8g2_GetFontCapitalAHeight(u8g2_t *u8g2) U8G2_NOINLINE; 
uint8_t u8g2_GetFontCapitalAHeight(u8g2_t *u8g2)
{
  return u8g2->font_info.ascent_A;		/* new font info structure */
}

/*========================================================================*/
/* glyph handling */

/* optimized */
/**
 * @brief   从 RLE 位流中按需取出 cnt 位（u8g2 字体解码的心脏，支持非字节对齐取位）
 * @param   f    解码状态机（decode_ptr 指当前字节，decode_bit_pos 为字节内位游标）
 * @param   cnt  要读取的位数 1~8（各字段位宽由字体头 bits_per_* 声明）
 * @return  取出的 cnt 位无符号值
 * @note    位流 LSB-first：位游标从字节低位向高位推进；cnt 跨字节边界时
 *          把下一字节左移拼接到高位，最后用掩码截取——最多两次 pgm 读完成任意取位。
 *          位流在 setup_decode 建立，随后被 x/y/dx/RLE 各字段跨函数连续消费
 */
uint8_t u8g2_font_decode_get_unsigned_bits(u8g2_font_decode_t *f, uint8_t cnt) 
{
  uint8_t val;
  uint8_t bit_pos = f->decode_bit_pos;
  uint8_t bit_pos_plus_cnt;
  
  //val = *(f->decode_ptr);
  val = u8x8_pgm_read( f->decode_ptr );  
  
  // 先按位游标右移，让待取位对齐到字节最低位
  val >>= bit_pos;
  bit_pos_plus_cnt = bit_pos;
  bit_pos_plus_cnt += cnt;
  // 本次取值越过字节边界：拼接下一字节的低位补齐高位
  if ( bit_pos_plus_cnt >= 8 )
  {
    uint8_t s = 8;
    s -= bit_pos;
    f->decode_ptr++;
    //val |= *(f->decode_ptr) << (8-bit_pos);
    val |= u8x8_pgm_read( f->decode_ptr ) << (s);
    //bit_pos -= 8;
    bit_pos_plus_cnt -= 8;
  }
  // 掩码截取 cnt 位（跨字节拼接后，高位可能混入下一字段的位）
  val &= (1U<<cnt)-1;
  //bit_pos += cnt;
  
  f->decode_bit_pos = bit_pos_plus_cnt;
  return val;
}


/*
    2 bit --> cnt = 2
      -2,-1,0. 1

    3 bit --> cnt = 3
      -2,-1,0. 1
      -4,-3,-2,-1,0,1,2,3

      if ( x < 0 )
	r = bits(x-1)+1;
    else
	r = bits(x)+1;

*/
/* optimized */
/**
 * @brief   取 cnt 位并按"偏移二进制码"还原为有符号数
 * @param   f    解码状态机
 * @param   cnt  位数（bits_per_char_x / bits_per_char_y / bits_per_delta_x 字段声明）
 * @return  有符号 8 位值 = 无符号原始值 - 2^(cnt-1)
 * @note    字形的 x/y 偏移与步进可为负：编译字体时存的是 (真值 + 2^(cnt-1))，
 *          这里做逆变换；例如 cnt=3 时编码 0..7 映射回 -4..3
 */
int8_t u8g2_font_decode_get_signed_bits(u8g2_font_decode_t *f, uint8_t cnt)
{
  int8_t v, d;
  v = (int8_t)u8g2_font_decode_get_unsigned_bits(f, cnt);
  d = 1;
  cnt--;
  d <<= cnt;
  // 减去偏置 2^(cnt-1)，把 [0, 2^cnt-1] 平移回有符号区间
  v -= d;
  return v;
  //return (int8_t)u8g2_font_decode_get_unsigned_bits(f, cnt) - ((1<<cnt)>>1);
}


#ifdef U8G2_WITH_FONT_ROTATION
/**
 * @brief   字体旋转时把字形局部坐标 (x,y) 映射为屏幕 Y 位移（仅 U8G2_WITH_FONT_ROTATION）
 * @param   dy   目标点 Y 基准值
 * @param   x,y  字形内局部坐标（单位：像素）
 * @param   dir  旋转方向 0/1/2/3 = 0°/90°/180°/270°
 * @return  旋转后的 Y 坐标
 * @note    dir=0: +y；dir=1（顺时针 90°）：局部 x 变成屏幕 y；dir=2: -y；dir=3: -x
 */
u8g2_uint_t u8g2_add_vector_y(u8g2_uint_t dy, int8_t x, int8_t y, uint8_t dir)
{
  switch(dir)
  {
    case 0:
      dy += y;
      break;
    case 1:
      dy += x;
      break;
    case 2:
      dy -= y;
      break;
    default:
      dy -= x;
      break;      
  }
  return dy;
}

/**
 * @brief   字体旋转时把字形局部坐标 (x,y) 映射为屏幕 X 位移（与 add_vector_y 配对使用）
 * @param   dx   目标点 X 基准值
 * @param   x,y  字形内局部坐标
 * @param   dir  旋转方向 0/1/2/3
 * @return  旋转后的 X 坐标
 * @note    与 add_vector_y 的符号表互补，二者合起来完成 2D 旋转变换（无三角函数，
 *          纯加减法实现，适配无 FPU 的 MCU）
 */
u8g2_uint_t u8g2_add_vector_x(u8g2_uint_t dx, int8_t x, int8_t y, uint8_t dir)
{
  switch(dir)
  {
    case 0:
      dx += x;
      break;
    case 1:
      dx -= y;
      break;
    case 2:
      dx -= x;
      break;
    default:
      dx += y;
      break;      
  }
  return dx;
}

/*
// does not make sense, 50 bytes more required on avr
void u8g2_add_vector(u8g2_uint_t *xp, u8g2_uint_t *yp, int8_t x, int8_t y, uint8_t dir)
{
  u8g2_uint_t x_ = *xp;
  u8g2_uint_t y_ = *yp;
  switch(dir)
  {
    case 0:
      y_ += y;
      x_ += x;
      break;
    case 1:
      y_ += x;
      x_ -= y;
      break;
    case 2:
      y_ -= y;
      x_ -= x;
      break;
    default:
      y_ -= x;
      x_ += y;
      break;      
  }
  *xp = x_;
  *yp = y_;
}
*/
#endif



/*
  Description:
    Draw a run-length area of the glyph. "len" can have any size and the line
    length has to be wrapped at the glyph border.
  Args:
    len: 					Length of the line
    is_foreground			foreground/background?
    u8g2->font_decode.target_x		X position
    u8g2->font_decode.target_y		Y position
    u8g2->font_decode.is_transparent	Transparent mode
  Return:
    -
  Calls:
    u8g2_Draw90Line()
  Called by:
    u8g2_font_decode_glyph()
*/
/* optimized */
/**
 * @brief   画一段 RLE 游程（len 个像素），自动处理跨行折返，透明/实心模式在此分叉
 * @param   u8g2          主对象（借用 font_decode 里的光标与颜色状态）
 * @param   len           本次游程长度（0/1 串的像素个数）
 * @param   is_foreground 1=画"1"位（前景色 fg_color），0=画"0"位（仅实心模式画 bg_color）
 * @note    字形位图逐行存放，一个游程可能从行中延续到下一行：
 *          rem = glyph_width - lx 是"到本行右边的余量"，不足则整行画完、ly++ 续行。
 *          前景无条件画；背景只在 is_transparent==0 时画（透明模式跳过 0 段）——
 *          这正是 u8g2_SetFontMode(0/1) 的全部效果所在。
 *          draw_color 是全局状态，画完整个字形后由 decode_glyph 恢复
 */
void u8g2_font_decode_len(u8g2_t *u8g2, uint8_t len, uint8_t is_foreground)
{
  uint8_t cnt;	/* total number of remaining pixels, which have to be drawn */
  uint8_t rem; 	/* remaining pixel to the right edge of the glyph */
  uint8_t current;	/* number of pixels, which need to be drawn for the draw procedure */
    /* current is either equal to cnt or equal to rem */
  
  /* local coordinates of the glyph */
  uint8_t lx,ly;
  
  /* target position on the screen */
  u8g2_uint_t x, y;
  
  u8g2_font_decode_t *decode = &(u8g2->font_decode);
  
  cnt = len;
  
  /* get the local position */
  lx = decode->x;
  ly = decode->y;
  
  for(;;)
  {
    /* calculate the number of pixel to the right edge of the glyph */
    rem = decode->glyph_width;
    rem -= lx;
    
    /* calculate how many pixel to draw. This is either to the right edge */
    /* or lesser, if not enough pixel are left */
    current = rem;
    if ( cnt < rem )
      current = cnt;
    
    
    /* now draw the line, but apply the rotation around the glyph target position */
    //u8g2_font_decode_draw_pixel(u8g2, lx,ly,current, is_foreground);

    /* get target position */
    x = decode->target_x;
    y = decode->target_y;

    /* apply rotation */
#ifdef U8G2_WITH_FONT_ROTATION
    
    // 把"字形局部坐标"经旋转变换叠加到屏幕目标点（dir=0 时等价于 x+=lx, y+=ly）
    x = u8g2_add_vector_x(x, lx, ly, decode->dir);
    y = u8g2_add_vector_y(y, lx, ly, decode->dir);
    
    //u8g2_add_vector(&x, &y, lx, ly, decode->dir);
    
    /* 未开启字体旋转时的直通路径：局部坐标就是屏幕偏移 */
#else
    x += lx;
    y += ly;
#endif
    
    /* draw foreground and background (if required) */
    if ( is_foreground )
    {
      u8g2->draw_color = decode->fg_color;			/* draw_color will be restored later */
      u8g2_DrawHVLine(u8g2, 
	x, 
	y, 
	current, 
#ifdef U8G2_WITH_FONT_ROTATION
	/* dir */ decode->dir
#else
	0
#endif
      );
    }
    else if ( decode->is_transparent == 0 )    
    {
      u8g2->draw_color = decode->bg_color;			/* draw_color will be restored later */
      u8g2_DrawHVLine(u8g2, 
	x, 
	y, 
	current, 
#ifdef U8G2_WITH_FONT_ROTATION
	/* dir */ decode->dir
#else
	0
#endif
      );   
    }
    
    /* check, whether the end of the run length code has been reached */
    if ( cnt < rem )
      break;
    cnt -= rem;
    lx = 0;
    ly++;
  }
  lx += cnt;
  
  decode->x = lx;
  decode->y = ly;  
}


/**
 * @brief   2 倍放大版的 u8g2_font_decode_len（供 DrawStrX2/DrawUTF8X2/DrawGlyphX2 系列）
 * @note    与原版唯一差别：局部坐标 x/y 均乘 2，且每段游程画两条相邻水平线
 *          （长度 current*2，分别在 y 与 y+1）——即"2x2 大像素"放大；
 *          不支持旋转（dir 恒 0）
 */
void u8g2_font_2x_decode_len(u8g2_t *u8g2, uint8_t len, uint8_t is_foreground)
{
  uint8_t cnt;	/* total number of remaining pixels, which have to be drawn */
  uint8_t rem; 	/* remaining pixel to the right edge of the glyph */
  uint8_t current;	/* number of pixels, which need to be drawn for the draw procedure */
    /* current is either equal to cnt or equal to rem */
  
  /* local coordinates of the glyph */
  uint8_t lx,ly;
  
  /* target position on the screen */
  u8g2_uint_t x, y;
  
  u8g2_font_decode_t *decode = &(u8g2->font_decode);
  
  cnt = len;
  
  /* get the local position */
  lx = decode->x;
  ly = decode->y;
  
  for(;;)
  {
    /* calculate the number of pixel to the right edge of the glyph */
    rem = decode->glyph_width;
    rem -= lx;
    
    /* calculate how many pixel to draw. This is either to the right edge */
    /* or lesser, if not enough pixel are left */
    current = rem;
    if ( cnt < rem )
      current = cnt;
    
    
    /* now draw the line, but apply the rotation around the glyph target position */
    //u8g2_font_decode_draw_pixel(u8g2, lx,ly,current, is_foreground);

    /* get target position */
    x = decode->target_x;
    y = decode->target_y;

    x += lx*2;
    y += ly*2;
    
    /* draw foreground and background (if required) */
    if ( is_foreground )
    {
      u8g2->draw_color = decode->fg_color;			/* draw_color will be restored later */
      u8g2_DrawHVLine(u8g2, 
	x, 
	y, 
	current*2, 
	0
      );
      u8g2_DrawHVLine(u8g2, 
	x, 
	y+1, 
	current*2, 
	0
      );
    }
    else if ( decode->is_transparent == 0 )    
    {
      u8g2->draw_color = decode->bg_color;			/* draw_color will be restored later */
      u8g2_DrawHVLine(u8g2, 
	x, 
	y, 
	current*2, 
	0
      );   
      u8g2_DrawHVLine(u8g2, 
	x, 
	y+1, 
	current*2, 
	0
      );   
    }
    
    /* check, whether the end of the run length code has been reached */
    if ( cnt < rem )
      break;
    cnt -= rem;
    lx = 0;
    ly++;
  }
  lx += cnt;
  
  decode->x = lx;
  decode->y = ly;
  
}


/**
 * @brief   初始化解码状态机：把 decode_ptr 指向字形数据并解出 width/height 头字段
 * @param   u8g2        主对象（font_decode、font_info 均挂在其上）
 * @param   glyph_data  指向该字形压缩数据首字节（调用前已跳过 encoding+size 前导）
 * @note    glyph 数据按位打包，顺序固定：[width: bits_per_char_width]
 *          [height: bits_per_char_height]，之后的 x/y/dx 在 decode_glyph 里继续读——
 *          位游标跨函数保持连续是整个解码设计的关键。
 *          顺带把当前 draw_color 缓存为前景色、取反为背景色
 */
static void u8g2_font_setup_decode(u8g2_t *u8g2, const uint8_t *glyph_data)
{
  u8g2_font_decode_t *decode = &(u8g2->font_decode);
  decode->decode_ptr = glyph_data;
  decode->decode_bit_pos = 0;
  
  /* 8 Nov 2015, this is already done in the glyph data search procedure */
  /*
  decode->decode_ptr += 1;
  decode->decode_ptr += 1;
  */
  
  // 解码顺序即位流顺序：先宽度、后高度，位游标随取位自动前进
  decode->glyph_width = u8g2_font_decode_get_unsigned_bits(decode, u8g2->font_info.bits_per_char_width);
  decode->glyph_height = u8g2_font_decode_get_unsigned_bits(decode,u8g2->font_info.bits_per_char_height);
  
  // 缓存前景色（=调用者当前 draw_color），背景色取反；解码结束后由 decode_glyph 恢复
  decode->fg_color = u8g2->draw_color;
  decode->bg_color = (decode->fg_color == 0 ? 1 : 0);
}


/*
  Description:
    Decode and draw a glyph.
  Args:
    glyph_data: 					Pointer to the compressed glyph data of the font
    u8g2->font_decode.target_x		X position
    u8g2->font_decode.target_y		Y position
    u8g2->font_decode.is_transparent	Transparent mode
  Return:
    Width (delta x advance) of the glyph.
  Calls:
    u8g2_font_decode_len()
*/
/* optimized */
/**
 * @brief   解码并立即绘制一个字形，返回该字形的步进宽度 dx
 * @param   u8g2       主对象（target_x/target_y/is_transparent/dir 需已就绪）
 * @param   glyph_data 压缩字形数据（由 u8g2_font_get_glyph_data 定位）
 * @return  字形步进 dx（像素）；字形位图宽为 0（如空格）时只返回 dx 不画
 * @note    全链路：setup_decode 解出 w/h → 读 x/y/dx 头 → 目标点修正
 *          （y 上移 h+y，即"基线到字形顶"）→ 与当前剪裁窗口求交
 *          （U8G2_WITH_INTERSECTION，不相交直接返回，省掉整段 RLE 解码）→
 *          主循环按 (a,b) 对解 RLE：a 个背景像素 + b 个前景像素，
 *          紧跟的 1 位是"同段继续"标志；局部 y 达到字形高度即结束
 */
int8_t u8g2_font_decode_glyph(u8g2_t *u8g2, const uint8_t *glyph_data)
{
  uint8_t a, b;
  int8_t x, y;
  int8_t d;
  int8_t h;
  u8g2_font_decode_t *decode = &(u8g2->font_decode);
    
  u8g2_font_setup_decode(u8g2, glyph_data);     /* set values in u8g2->font_decode data structure */
  h = u8g2->font_decode.glyph_height;
  
  x = u8g2_font_decode_get_signed_bits(decode, u8g2->font_info.bits_per_char_x);
  y = u8g2_font_decode_get_signed_bits(decode, u8g2->font_info.bits_per_char_y);
  d = u8g2_font_decode_get_signed_bits(decode, u8g2->font_info.bits_per_delta_x);
  
  if ( decode->glyph_width > 0 )
  {
#ifdef U8G2_WITH_FONT_ROTATION
    decode->target_x = u8g2_add_vector_x(decode->target_x, x, -(h+y), decode->dir);
    decode->target_y = u8g2_add_vector_y(decode->target_y, x, -(h+y), decode->dir);
    
    //u8g2_add_vector(&(decode->target_x), &(decode->target_y), x, -(h+y), decode->dir);

#else
    // 目标点修正：x 加字形左偏移；y 上移"字形高 + y 偏移"（U8g2 坐标系 y 向下）
    decode->target_x += x;
    decode->target_y -= h+y;
#endif
    //u8g2_add_vector(&(decode->target_x), &(decode->target_y), x, -(h+y), decode->dir);

#ifdef U8G2_WITH_INTERSECTION
    {
      u8g2_uint_t x0, x1, y0, y1;
      x0 = decode->target_x;
      y0 = decode->target_y;
      x1 = x0;
      y1 = y0;
      
#ifdef U8G2_WITH_FONT_ROTATION
      switch(decode->dir)
      {
	case 0:
	    x1 += decode->glyph_width;
	    y1 += h;
	    break;
	case 1:
	    x0 -= h;
	    x0++;	/* shift down, because of assymetric boundaries for the interseciton test */
	    x1++;
	    y1 += decode->glyph_width;
	    break;
	case 2:
	    x0 -= decode->glyph_width;
	    x0++;	/* shift down, because of assymetric boundaries for the interseciton test */
	    x1++;
	    y0 -= h;
	    y0++;	/* shift down, because of assymetric boundaries for the interseciton test */
	    y1++;
	    break;	  
	case 3:
	    x1 += h;
	    y0 -= decode->glyph_width;
	    y0++;	/* shift down, because of assymetric boundaries for the interseciton test */
	    y1++;
	    break;	  
      }
#else /* U8G2_WITH_FONT_ROTATION */
      x1 += decode->glyph_width;
      y1 += h;      
#endif
      
      if ( u8g2_IsIntersection(u8g2, x0, y0, x1, y1) == 0 ) 
	return d;
    }
#endif /* U8G2_WITH_INTERSECTION */
   
    /* reset local x/y position */
    decode->x = 0;
    decode->y = 0;
    
    /* decode glyph */
    for(;;)
    {
      a = u8g2_font_decode_get_unsigned_bits(decode, u8g2->font_info.bits_per_0);
      b = u8g2_font_decode_get_unsigned_bits(decode, u8g2->font_info.bits_per_1);
      // 一轮 = 一对游程：先 a 个背景（0）再 b 个前景（1）；
      // 紧随的 1 位为 1 表示同一行还有下一对 a/b，为 0 则进入下行
      do
      {
	u8g2_font_decode_len(u8g2, a, 0);
	u8g2_font_decode_len(u8g2, b, 1);
      } while( u8g2_font_decode_get_unsigned_bits(decode, 1) != 0 );

      if ( decode->y >= h )
	break;
    }
    
    /* restore the u8g2 draw color, because this is modified by the decode algo */
    u8g2->draw_color = decode->fg_color;
  }
  return d;
}


/**
 * @brief   2 倍放大版字形解码（配合 u8g2_font_2x_decode_len），返回 2*dx
 * @note    与原版差别：目标点 y -= 2*h + y_offset；剪裁框按放大后的 2*w × 2*h 计算；
 *          不支持旋转
 */
int8_t u8g2_font_2x_decode_glyph(u8g2_t *u8g2, const uint8_t *glyph_data)
{
  uint8_t a, b;
  int8_t x, y;
  int8_t d;
  int8_t h;
  u8g2_font_decode_t *decode = &(u8g2->font_decode);
    
  u8g2_font_setup_decode(u8g2, glyph_data);     /* set values in u8g2->font_decode data structure */
  h = u8g2->font_decode.glyph_height;
  
  x = u8g2_font_decode_get_signed_bits(decode, u8g2->font_info.bits_per_char_x);
  y = u8g2_font_decode_get_signed_bits(decode, u8g2->font_info.bits_per_char_y);
  d = u8g2_font_decode_get_signed_bits(decode, u8g2->font_info.bits_per_delta_x);
  
  if ( decode->glyph_width > 0 )
  {
    decode->target_x += x;
    decode->target_y -= 2*h+y;

#ifdef U8G2_WITH_INTERSECTION
    {
      u8g2_uint_t x0, x1, y0, y1;
      x0 = decode->target_x;
      y0 = decode->target_y;
      x1 = x0;
      y1 = y0;
      
      x1 += 2*decode->glyph_width;
      y1 += 2*h;      
      
      if ( u8g2_IsIntersection(u8g2, x0, y0, x1, y1) == 0 ) 
	return 2*d;
    }
#endif /* U8G2_WITH_INTERSECTION */
   
    /* reset local x/y position */
    decode->x = 0;
    decode->y = 0;
    
    /* decode glyph */
    for(;;)
    {
      a = u8g2_font_decode_get_unsigned_bits(decode, u8g2->font_info.bits_per_0);
      b = u8g2_font_decode_get_unsigned_bits(decode, u8g2->font_info.bits_per_1);
      do
      {
	u8g2_font_2x_decode_len(u8g2, a, 0);
	u8g2_font_2x_decode_len(u8g2, b, 1);
      } while( u8g2_font_decode_get_unsigned_bits(decode, 1) != 0 );

      if ( decode->y >= h )
	break;
    }
    
    /* restore the u8g2 draw color, because this is modified by the decode algo */
    u8g2->draw_color = decode->fg_color;
  }
  return d*2;
}

/*
  Description:
    Find the starting point of the glyph data.
  Args:
    encoding: Encoding (ASCII or Unicode) of the glyph
  Return:
    Address of the glyph data or NULL, if the encoding is not avialable in the font.
*/
/**
 * @brief   在字体数组中定位 encoding 对应的字形数据（glyph 寻址核心）
 * @param   u8g2     主对象（使用 u8g2->font 与 font_info 中的分段起点）
 * @param   encoding 字符编码：<=255 走 ASCII 分段线性链，>255 走 unicode 查找表
 * @return  指向该字形压缩数据的指针（已跳过 encoding+size 前导字节）；找不到返回 NULL
 * @note    字体 glyph 区是一条"隐式单链表"：ASCII 项每项第 2 字节 = 到下一项的字节距离
 *          （为 0 表示链尾）；unicode 项为大端 2 字节编码 + 1 字节距离。
 *          'A'/'a' 两个分段点（字体头 offset 17/19）把 ASCII 区截成 3 段，
 *          查找只扫所在段；unicode 区前置 4 字节/项的索引表（偏移+编码上界）先跳段再线性扫
 */
const uint8_t *u8g2_font_get_glyph_data(u8g2_t *u8g2, uint16_t encoding)
{
  const uint8_t *font = u8g2->font;
  font += U8G2_FONT_DATA_STRUCT_SIZE;

  
  if ( encoding <= 255 )
  {
    if ( encoding >= 'a' )
    {
      // ASCII >= 'a'：跳到小写段起点，从这段开始扫
      font += u8g2->font_info.start_pos_lower_a;
    }
    else if ( encoding >= 'A' )
    {
      // ASCII >= 'A' 且 < 'a'：跳到大写段起点
      font += u8g2->font_info.start_pos_upper_A;
    }
    
    for(;;)
    {
      if ( u8x8_pgm_read( font + 1 ) == 0 )
	break;
      if ( u8x8_pgm_read( font ) == encoding )
      {
	return font+2;	/* skip encoding and glyph size */
      }
      font += u8x8_pgm_read( font + 1 );
    }
  }
#ifdef U8G2_WITH_UNICODE
  else
  {
    uint16_t e;
    const uint8_t *unicode_lookup_table;
    
// removed, there is now the new index table
//#ifdef  __unix__
//    if ( u8g2->last_font_data != NULL && encoding >= u8g2->last_unicode )
//    {
//	font = u8g2->last_font_data;
//    }
//    else
//#endif 

    font += u8g2->font_info.start_pos_unicode;
    unicode_lookup_table = font; 
  
    /* issue 596: search for the glyph start in the unicode lookup table */
    // unicode 索引表每项 4 字节：[2 字节段偏移][2 字节编码上界（大端）]，
    // 逐步前进直到某段的编码上界 >= 目标编码，再在该段内线性扫
    do
    {
      font += u8g2_font_get_word(unicode_lookup_table, 0);
      e = u8g2_font_get_word(unicode_lookup_table, 2);
      unicode_lookup_table+=4;
    } while( e < encoding );
    
  
    for(;;)
    {
      e = u8x8_pgm_read( font );
      e <<= 8;
      e |= u8x8_pgm_read( font + 1 );
  
// removed, there is now the new index table  
//#ifdef  __unix__
//      if ( encoding < e )
//        break;
//#endif 

      if ( e == 0 )
	break;
  
      if ( e == encoding )
      {
// removed, there is now the new index table
//#ifdef  __unix__
//	u8g2->last_font_data = font;
//	u8g2->last_unicode = encoding;
//#endif 
	return font+3;	/* skip encoding and glyph size */
      }
      font += u8x8_pgm_read( font + 2 );
    }  
  }
#endif
  
  return NULL;
}

/**
 * @brief   单字形绘制入口：寻址 + 解码绘制，返回步进宽度
 * @param   x,y      基准点（通常已经过 font_calc_vref 参考点修正，见 u8g2_DrawGlyph）
 * @param   encoding 字符编码
 * @return  字形步进 dx；字形不存在时返回 0（字符串会"跳过"缺失字符继续排）
 * @note    is_transparent/dir 由上层 SetFontMode/SetFontDirection 预先写进 font_decode，
 *          本函数只负责"定位 + 解码"两步
 */
static u8g2_uint_t u8g2_font_draw_glyph(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, uint16_t encoding)
{
  u8g2_uint_t dx = 0;
  u8g2->font_decode.target_x = x;
  u8g2->font_decode.target_y = y;
  //u8g2->font_decode.is_transparent = is_transparent; this is already set
  //u8g2->font_decode.dir = dir;
  const uint8_t *glyph_data = u8g2_font_get_glyph_data(u8g2, encoding);
  if ( glyph_data != NULL )
  {
    dx = u8g2_font_decode_glyph(u8g2, glyph_data);
  }
  return dx;
}

/**
 * @brief   2 倍放大版单字形绘制入口（逻辑同 u8g2_font_draw_glyph，走 2x 解码路径）
 */
static u8g2_uint_t u8g2_font_2x_draw_glyph(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, uint16_t encoding)
{
  u8g2_uint_t dx = 0;
  u8g2->font_decode.target_x = x;
  u8g2->font_decode.target_y = y;
  const uint8_t *glyph_data = u8g2_font_get_glyph_data(u8g2, encoding);
  if ( glyph_data != NULL )
  {
    dx = u8g2_font_2x_decode_glyph(u8g2, glyph_data);
  }
  return dx;
}



/**
 * @brief   查询当前字体是否包含指定编码的字形
 * @param   requested_encoding 字符编码
 * @return  1=存在，0=不存在
 * @note    只做寻址不做解码，供字体回退/输入法过滤等逻辑判断字体覆盖范围
 */
uint8_t u8g2_IsGlyph(u8g2_t *u8g2, uint16_t requested_encoding)
{
  /* updated to new code */
  if ( u8g2_font_get_glyph_data(u8g2, requested_encoding) != NULL )
    return 1;
  return 0;
}

/* side effect: updates u8g2->font_decode and u8g2->glyph_x_offset */
/**
 * @brief   返回字形步进宽度 dx；同时以"副作用"更新 glyph_x_offset 与 font_decode.glyph_width
 * @param   requested_encoding 字符编码
 * @return  步进宽度 dx（像素）；字形不存在返回 0
 * @note    副作用是有意设计：u8g2_string_width 计算串宽时正好要用 glyph_width 与
 *          x_offset 做"末字符修正"（issue #16/#46），避免二次寻址解码
 */
int8_t u8g2_GetGlyphWidth(u8g2_t *u8g2, uint16_t requested_encoding)
{
  const uint8_t *glyph_data = u8g2_font_get_glyph_data(u8g2, requested_encoding);
  if ( glyph_data == NULL )
    return 0; 
  
  u8g2_font_setup_decode(u8g2, glyph_data);
  u8g2->glyph_x_offset = u8g2_font_decode_get_signed_bits(&(u8g2->font_decode), u8g2->font_info.bits_per_char_x);
  u8g2_font_decode_get_signed_bits(&(u8g2->font_decode), u8g2->font_info.bits_per_char_y);
  
  /* glyph width is here: u8g2->font_decode.glyph_width */

  return u8g2_font_decode_get_signed_bits(&(u8g2->font_decode), u8g2->font_info.bits_per_delta_x);
}


/*
  set one of:
    U8G2_FONT_MODE_TRANSPARENT
    U8G2_FONT_MODE_SOLID
    U8G2_FONT_MODE_NONE
  This has been changed for the new font procedures  
*/
/**
 * @brief   设置字体绘制模式：1=透明（"0"位不画），0=实心（"0"位画背景色）
 * @param   is_transparent U8G2_FONT_MODE_TRANSPARENT(1) / U8G2_FONT_MODE_SOLID(0)
 * @note    只写一个标志位，真正的分叉在 u8g2_font_decode_len；
 *          "t" 后缀字体（proportional）官方只保证透明模式下显示正确
 */
void u8g2_SetFontMode(u8g2_t *u8g2, uint8_t is_transparent)
{
  u8g2->font_decode.is_transparent = is_transparent;		// new font procedures
}

/**
 * @brief   画单个字形（绕过字符串管线，直接给编码）
 * @param   x,y      基准点；实际绘制位置会先叠加 font_calc_vref() 的参考点修正
 *                   （由 SetFontPosBaseline/Top/Bottom/Center 决定语义）
 * @param   encoding 字符编码（ASCII 或 Unicode 码点）
 * @return  字形步进 dx
 * @note    4 个旋转方向下 vref 分别加到 y/x/-y/-x，保证"参考点"语义随方向一致；
 *          中文等直接给码点绘制（配合 DrawUTF8 已解码场景）可用本接口
 */
u8g2_uint_t u8g2_DrawGlyph(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, uint16_t encoding)
{
#ifdef U8G2_WITH_FONT_ROTATION
  switch(u8g2->font_decode.dir)
  {
    case 0:
      y += u8g2->font_calc_vref(u8g2);
      break;
    case 1:
      x -= u8g2->font_calc_vref(u8g2);
      break;
    case 2:
      y -= u8g2->font_calc_vref(u8g2);
      break;
    case 3:
      x += u8g2->font_calc_vref(u8g2);
      break;
  }
#else
  y += u8g2->font_calc_vref(u8g2);
#endif
  return u8g2_font_draw_glyph(u8g2, x, y, encoding);
}

/**
 * @brief   2 倍放大画单字形（参考点修正也乘 2），见 u8g2_font_2x_decode_glyph
 */
u8g2_uint_t u8g2_DrawGlyphX2(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, uint16_t encoding)
{
  y += 2*u8g2->font_calc_vref(u8g2);
  return u8g2_font_2x_draw_glyph(u8g2, x, y, encoding);
}

/**
 * @brief   字符串绘制主循环：逐字节喂给 next_cb 取码点 → DrawGlyph → 沿书写方向累加步进
 * @param   x,y 起始基准点
 * @param   str 待绘字符串（编码方式由 u8x8->next_cb 决定：ascii_next 或 utf8_next）
 * @return  整串沿书写方向的总步进
 * @note    next_cb 是字节→码点状态机：返回 0xffff=串结束；0xfffe=UTF-8 多字节
 *          序列中间态（跳过不画）。旋转开启时光标推进方向随 dir 改变，
 *          否则固定 x += dx
 */
static u8g2_uint_t u8g2_draw_string(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, const char *str) U8G2_NOINLINE;
static u8g2_uint_t u8g2_draw_string(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, const char *str)
{
  uint16_t e;
  u8g2_uint_t delta, sum;
  u8x8_utf8_init(u8g2_GetU8x8(u8g2));
  sum = 0;
  for(;;)
  {
    e = u8g2->u8x8.next_cb(u8g2_GetU8x8(u8g2), (uint8_t)*str);
    if ( e == 0x0ffff )
      break;
    str++;
    // 0xffff=串结束；0xfffe=UTF-8 前导/中间字节，等后续字节凑齐再出码点
    if ( e != 0x0fffe )
    {
      delta = u8g2_DrawGlyph(u8g2, x, y, e);
    
#ifdef U8G2_WITH_FONT_ROTATION
      // 沿书写方向推进光标：dir 决定 x/y 谁增谁减
      switch(u8g2->font_decode.dir)
      {
	case 0:
	  x += delta;
	  break;
	case 1:
	  y += delta;
	  break;
	case 2:
	  x -= delta;
	  break;
	case 3:
	  y -= delta;
	  break;
      }
      
      /*
      // requires 10 bytes more on avr
      x = u8g2_add_vector_x(x, delta, 0, u8g2->font_decode.dir);
      y = u8g2_add_vector_y(y, delta, 0, u8g2->font_decode.dir);
      */

#else
      x += delta;
#endif

      sum += delta;    
    }
  }
  return sum;
}

/**
 * @brief   2 倍放大版字符串主循环（无旋转支持），字符间以 2*dx 步进
 */
static u8g2_uint_t u8g2_draw_string_2x(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, const char *str) U8G2_NOINLINE;
static u8g2_uint_t u8g2_draw_string_2x(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, const char *str)
{
  uint16_t e;
  u8g2_uint_t delta, sum;
  u8x8_utf8_init(u8g2_GetU8x8(u8g2));
  sum = 0;
  for(;;)
  {
    e = u8g2->u8x8.next_cb(u8g2_GetU8x8(u8g2), (uint8_t)*str);
    if ( e == 0x0ffff )
      break;
    str++;
    if ( e != 0x0fffe )
    {
      delta = u8g2_DrawGlyphX2(u8g2, x, y, e);
      x += delta;
      sum += delta;    
    }
  }
  return sum;
}

/**
 * @brief   画 ASCII 字符串（挂 u8x8_ascii_next 回调后进 u8g2_draw_string）
 * @param   x,y 基准点（参考点语义由 SetFontPos* 决定，默认基线）
 * @param   str 以 NUL 结尾的 ASCII/扩展 ASCII 字符串
 * @return  字符串宽度（总步进，像素）
 */
u8g2_uint_t u8g2_DrawStr(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, const char *str)
{
  u8g2->u8x8.next_cb = u8x8_ascii_next;
  return u8g2_draw_string(u8g2, x, y, str);
}

/**
 * @brief   2 倍放大画 ASCII 字符串（无旋转、无 kerning，字体数据不变仅渲染放大）
 */
u8g2_uint_t u8g2_DrawStrX2(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, const char *str)
{
  u8g2->u8x8.next_cb = u8x8_ascii_next;
  return u8g2_draw_string_2x(u8g2, x, y, str);
}

/*
source: https://en.wikipedia.org/wiki/UTF-8
Bits	from 		to			bytes	Byte 1 		Byte 2 		Byte 3 		Byte 4 		Byte 5 		Byte 6
  7 	U+0000 		U+007F 		1 		0xxxxxxx
11 	U+0080 		U+07FF 		2 		110xxxxx 	10xxxxxx
16 	U+0800 		U+FFFF 		3 		1110xxxx 	10xxxxxx 	10xxxxxx
21 	U+10000 	U+1FFFFF 	4 		11110xxx 	10xxxxxx 	10xxxxxx 	10xxxxxx
26 	U+200000 	U+3FFFFFF 	5 		111110xx 	10xxxxxx 	10xxxxxx 	10xxxxxx 	10xxxxxx
31 	U+4000000 	U+7FFFFFFF 	6 		1111110x 	10xxxxxx 	10xxxxxx 	10xxxxxx 	10xxxxxx 	10xxxxxx  
*/
/**
 * @brief   画 UTF-8 字符串（挂 u8x8_utf8_next 回调解码多字节序列）
 * @return  字符串宽度
 * @note    需字体包含对应 Unicode 字形（如 u8g2_font_wqy* 中文字体）；
 *          未覆盖的码点被静默跳过（get_glyph_data 返回 NULL → dx=0），
 *          显示前可用 u8g2_IsAllValidUTF8 预检
 */
u8g2_uint_t u8g2_DrawUTF8(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, const char *str)
{
  u8g2->u8x8.next_cb = u8x8_utf8_next;
  return u8g2_draw_string(u8g2, x, y, str);
}

/**
 * @brief   2 倍放大画 UTF-8 字符串
 */
u8g2_uint_t u8g2_DrawUTF8X2(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, const char *str)
{
  u8g2->u8x8.next_cb = u8x8_utf8_next;
  return u8g2_draw_string_2x(u8g2, x, y, str);
}


/**
 * @brief   带 kerning（字距修正）的 UTF-8 绘制，支持从左或从右排版
 * @param   to_left  1=向左排（阿拉伯语等 RTL 文字），x 逐字符左移
 * @param   kerning  字距查询对象（实现在 u8g2_kerning.c，按相邻码点对查表）
 * @param   str      UTF-8 字符串
 * @return  整串总宽
 * @note    每字符先查步进 dx，再减去与前一字符的字距 k；RTL 分支先扣宽度再画，
 *          LTR 分支先推进光标再画——两条路径保持"光标停在最后一字之后"的约定。
 *          e_prev 初值 0xffff 保证首字符查不到 kerning（间距修正为 0）
 */
u8g2_uint_t u8g2_DrawExtendedUTF8(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, uint8_t to_left, u8g2_kerning_t *kerning, const char *str)
{
  u8g2->u8x8.next_cb = u8x8_utf8_next;
  uint16_t e_prev = 0x0ffff;
  uint16_t e;
  u8g2_uint_t delta, sum, k;
  u8x8_utf8_init(u8g2_GetU8x8(u8g2));
  sum = 0;
  for(;;)
  {
    e = u8g2->u8x8.next_cb(u8g2_GetU8x8(u8g2), (uint8_t)*str);
    if ( e == 0x0ffff )
      break;
    str++;
    if ( e != 0x0fffe )
    {
      delta = u8g2_GetGlyphWidth(u8g2, e);
	    
      if ( to_left )
      {
        // kerning 按相邻字符对 (prev, cur) 查表；等宽/中文字体通常返回 0
        k = u8g2_GetKerning(u8g2, kerning, e, e_prev);
	delta -= k;
	x -= delta;
      }
      else
      {
        k = u8g2_GetKerning(u8g2, kerning, e_prev, e);
	delta -= k;
      }
      e_prev = e;

      u8g2_DrawGlyph(u8g2, x, y, e);
      if ( to_left )
      {
      }
      else
      {
	x += delta;
	x -= k;
      }
      
      sum += delta;    
    }
  }
  return sum;
}

/**
 * @brief   同 DrawExtendedUTF8，但字距来自扁平的 16 位查找表（hbshape2u8g2 工具产物）
 * @param   kerning_table  三元组连续存放：[左编码][右编码][字距]，以全 0 项结尾
 * @note    与 u8g2_kerning_t 版本相比不需要 RAM 中的树结构，适合只读 Flash 表；
 *          查询走 u8g2_GetKerningByTable
 */
u8g2_uint_t u8g2_DrawExtUTF8(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, uint8_t to_left, const uint16_t *kerning_table, const char *str)
{
  u8g2->u8x8.next_cb = u8x8_utf8_next;
  uint16_t e_prev = 0x0ffff;
  uint16_t e;
  u8g2_uint_t delta, sum, k;
  u8x8_utf8_init(u8g2_GetU8x8(u8g2));
  sum = 0;
  for(;;)
  {
    e = u8g2->u8x8.next_cb(u8g2_GetU8x8(u8g2), (uint8_t)*str);
    if ( e == 0x0ffff )
      break;
    str++;
    if ( e != 0x0fffe )
    {
      delta = u8g2_GetGlyphWidth(u8g2, e);
	    
      if ( to_left )
      {
        k = u8g2_GetKerningByTable(u8g2, kerning_table, e, e_prev);
	delta -= k;
	x -= delta;
      }
      else
      {
        k = u8g2_GetKerningByTable(u8g2, kerning_table, e_prev, e);
	delta -= k;
      }
      e_prev = e;

      if ( to_left )
      {
      }
      else
      {
	x += delta;
      }
      u8g2_DrawGlyph(u8g2, x, y, e);
      if ( to_left )
      {
      }
      else
      {
	//x += delta;
	//x -= k;
      }
      
      sum += delta;    
    }
  }
  return sum;
}



/*===============================================*/

/* set ascent/descent for reference point calculation */

/**
 * @brief   按 height_mode 重算参考上/下沿 font_ref_ascent / font_ref_descent
 * @note    三档模式的取值来源（决定 vref 换算与多行行距）：
 *          TEXT  —— 大写 A 上沿 / 小写 g 下沿（默认，行距紧凑）
 *          XTEXT —— 并入括号 ( ) 的极值，覆盖更多字形
 *          ALL   —— max_char_height+y_offset 全包络，任何字形都不越界（最保守）
 *          换字体（SetFont）或换模式（SetFontRefHeight*）都会触发本函数
 */
void u8g2_UpdateRefHeight(u8g2_t *u8g2)
{
  if ( u8g2->font == NULL )
    return;
  u8g2->font_ref_ascent = u8g2->font_info.ascent_A;
  u8g2->font_ref_descent = u8g2->font_info.descent_g;
  if ( u8g2->font_height_mode == U8G2_FONT_HEIGHT_MODE_TEXT )
  {
  }
  else if ( u8g2->font_height_mode == U8G2_FONT_HEIGHT_MODE_XTEXT )
  {
    if ( u8g2->font_ref_ascent < u8g2->font_info.ascent_para )
      u8g2->font_ref_ascent = u8g2->font_info.ascent_para;
    if ( u8g2->font_ref_descent > u8g2->font_info.descent_para )
      u8g2->font_ref_descent = u8g2->font_info.descent_para;
  }
  else
  {
    if ( u8g2->font_ref_ascent < u8g2->font_info.max_char_height+u8g2->font_info.y_offset )
      u8g2->font_ref_ascent = u8g2->font_info.max_char_height+u8g2->font_info.y_offset;
    if ( u8g2->font_ref_descent > u8g2->font_info.y_offset )
      u8g2->font_ref_descent = u8g2->font_info.y_offset;
  }  
}

/**
 * @brief   参考高度取"大写 A / 小写 g"（TEXT 档，默认；行距最紧凑）
 */
void u8g2_SetFontRefHeightText(u8g2_t *u8g2)
{
  u8g2->font_height_mode = U8G2_FONT_HEIGHT_MODE_TEXT;
  u8g2_UpdateRefHeight(u8g2);
}

/**
 * @brief   参考高度并入括号 ( ) 的极值（XTEXT 档，介于 TEXT 与 ALL 之间）
 */
void u8g2_SetFontRefHeightExtendedText(u8g2_t *u8g2)
{
  u8g2->font_height_mode = U8G2_FONT_HEIGHT_MODE_XTEXT;
  u8g2_UpdateRefHeight(u8g2);
}

/**
 * @brief   参考高度取字体最大包络（ALL 档；保证任何字形都被参考框覆盖）
 */
void u8g2_SetFontRefHeightAll(u8g2_t *u8g2)
{
  u8g2->font_height_mode = U8G2_FONT_HEIGHT_MODE_ALL;
  u8g2_UpdateRefHeight(u8g2);
}

/*===============================================*/
/* callback procedures to correct the y position */

/**
 * @brief   基线模式的 vref 回调：恒返回 0（y 参数即基线，不做修正）
 * @note    u8g2 用"函数指针 + 小回调"实现参考点策略，避免在绘制路径里写 switch
 */
u8g2_uint_t u8g2_font_calc_vref_font(U8X8_UNUSED u8g2_t *u8g2)
{
  return 0;
}

/**
 * @brief   参考点 = 字符基线（默认模式；x,y 中的 y 就是基线位置）
 */
void u8g2_SetFontPosBaseline(u8g2_t *u8g2)
{
  u8g2->font_calc_vref = u8g2_font_calc_vref_font;
}


/**
 * @brief   底部模式的 vref 回调：返回 font_ref_descent（通常为负，把 y 顶到字底边）
 */
u8g2_uint_t u8g2_font_calc_vref_bottom(u8g2_t *u8g2)
{
  return (u8g2_uint_t)(u8g2->font_ref_descent);
}

/**
 * @brief   参考点 = 字体下沿（y 是文字底部；配合 DrawFrame 底对齐排版很方便）
 */
void u8g2_SetFontPosBottom(u8g2_t *u8g2)
{
  u8g2->font_calc_vref = u8g2_font_calc_vref_bottom;
}

/**
 * @brief   顶部模式的 vref 回调：返回 font_ref_ascent + 1（参考点在字顶再上一行）
 * @note    +1 的原因：参考点语义是"文字从该行下方开始"，与 bottom 模式对称
 */
u8g2_uint_t u8g2_font_calc_vref_top(u8g2_t *u8g2)
{
  u8g2_uint_t tmp;
  /* reference pos is one pixel above the upper edge of the reference glyph */
  tmp = (u8g2_uint_t)(u8g2->font_ref_ascent);
  tmp++;
  return tmp;
}

/**
 * @brief   参考点 = 字体上沿（y 是文字顶部所在行）
 */
void u8g2_SetFontPosTop(u8g2_t *u8g2)
{
  u8g2->font_calc_vref = u8g2_font_calc_vref_top;
}

/**
 * @brief   居中模式的 vref 回调：返回 (ascent - descent)/2 + descent
 * @note    纯整数运算的中点公式：先算"总高一半相对 descent 的偏移"再加回 descent，
 *          对负的 descent 也成立；常用于把文字垂直居中在图标/按钮里
 */
u8g2_uint_t u8g2_font_calc_vref_center(u8g2_t *u8g2)
{
  int8_t tmp;
  tmp = u8g2->font_ref_ascent;
  tmp -= u8g2->font_ref_descent;
  tmp /= 2;
  tmp += u8g2->font_ref_descent;  
  return tmp;
}

/**
 * @brief   参考点 = 文字垂直居中线
 */
void u8g2_SetFontPosCenter(u8g2_t *u8g2)
{
  u8g2->font_calc_vref = u8g2_font_calc_vref_center;
}

/*===============================================*/

/**
 * @brief   选择当前字体并解析字体头（同字体重复设置直接返回，做了一次缓存判断）
 * @param   font 字体数组（bdfconv 生成的 C 数组，通常放在 Flash）
 * @note    切换时重读 font_info 并刷新参考高度；从 issue 195 起不再重置参考点模式，
 *          即 SetFontPos* 的选择跨字体保留。font_info 必须先于任何 Draw/Get 宽度
 *          调用就绪，否则解码位宽全错
 */
void u8g2_SetFont(u8g2_t *u8g2, const uint8_t  *font)
{
  if ( u8g2->font != font )
  {
//#ifdef  __unix__
//	u8g2->last_font_data = NULL;
//	u8g2->last_unicode = 0x0ffff;
//#endif 
    u8g2->font = font;
    u8g2_read_font_info(&(u8g2->font_info), font);
    u8g2_UpdateRefHeight(u8g2);
    /* u8g2_SetFontPosBaseline(u8g2); */ /* removed with issue 195 */
  }
}

/*===============================================*/

/**
 * @brief   检查字符串中每个码点在当前字体里是否都有字形
 * @param   str 待检字符串（编码方式由 next_cb 决定）
 * @return  1=全部可用，0=至少一个缺失（遇到第一个缺失立即返回）
 * @note    显示前预检可避免"静默丢字"；配合 u8g2_IsAllValidUTF8 做整串校验
 */
static uint8_t u8g2_is_all_valid(u8g2_t *u8g2, const char *str) U8G2_NOINLINE;
static uint8_t u8g2_is_all_valid(u8g2_t *u8g2, const char *str)
{
  uint16_t e;
  u8x8_utf8_init(u8g2_GetU8x8(u8g2));
  for(;;)
  {
    e = u8g2->u8x8.next_cb(u8g2_GetU8x8(u8g2), (uint8_t)*str);
    if ( e == 0x0ffff )
      break;
    str++;
    if ( e != 0x0fffe )
    {
      if ( u8g2_font_get_glyph_data(u8g2, e) == NULL )
	return 0;
    }
  }
  return 1;
}

/**
 * @brief   UTF-8 版全量字形检查：先挂 utf8 解码回调再复用 is_all_valid
 * @return  1=串内所有码点都有字形；0=有缺失
 */
uint8_t u8g2_IsAllValidUTF8(u8g2_t *u8g2, const char *str)
{
  u8g2->u8x8.next_cb = u8x8_utf8_next;
  return u8g2_is_all_valid(u8g2, str);
}


/* string calculation is stilll not 100% perfect as it addes the initial string offset to the overall size */
/**
 * @brief   计算字符串像素宽度（DrawStr/DrawUTF8/GetStrWidth/GetUTF8Width 的公共底层）
 * @return  字符串宽度（像素）
 * @note    基础宽度 = Σ(各字符步进 dx)；随后做三步"末字符修正"（仅当末字形位图宽
 *          非 0，issue #16 空格不修正）：① 把末字符的 dx 换成实际位图宽 glyph_width；
 *          ② 补上末字符的 x_offset（issue #46）；③ 首字符 x_offset 为正时补回
 *          （issue #1561，补偿首字形右侧留白对整体观感的影响）。
 *          全程只解字形头位段、不解 RLE 位图，复杂度 O(串长)
 */
static u8g2_uint_t u8g2_string_width(u8g2_t *u8g2, const char *str) U8G2_NOINLINE;
static u8g2_uint_t u8g2_string_width(u8g2_t *u8g2, const char *str)
{
  uint16_t e;
  u8g2_uint_t  w, dx;
#ifdef U8G2_BALANCED_STR_WIDTH_CALCULATION
  int8_t initial_x_offset = -64;
#endif 
  
  u8g2->font_decode.glyph_width = 0;
  u8x8_utf8_init(u8g2_GetU8x8(u8g2));
  
  /* reset the total width to zero, this will be expanded during calculation */
  w = 0;
  dx = 0;

  // printf("str=<%s>\n", str);
	
  for(;;)
  {
    e = u8g2->u8x8.next_cb(u8g2_GetU8x8(u8g2), (uint8_t)*str);
    if ( e == 0x0ffff )
      break;
    str++;
    if ( e != 0x0fffe )
    {
      // 副作用：同时更新 u8g2->glyph_x_offset 与 font_decode.glyph_width
      dx = u8g2_GetGlyphWidth(u8g2, e);		/* delta x value of the glyph, side effect: updates u8g2->glyph_x_offset */
#ifdef U8G2_BALANCED_STR_WIDTH_CALCULATION
      if ( initial_x_offset == -64 )
        initial_x_offset = u8g2->glyph_x_offset;
#endif 
      //printf("'%c' x=%d dx=%d w=%d io=%d ", e, u8g2->glyph_x_offset, dx, u8g2->font_decode.glyph_width, initial_x_offset);
      w += dx;
    }
  }
  //printf("\n");
  
  /* adjust the last glyph, check for issue #16: do not adjust if width is 0 */
  // 末字形位图宽非 0 才做修正（issue #16：末字符是空格等"零宽位图"时保持 dx 累加值）
  if ( u8g2->font_decode.glyph_width != 0 )
  {
    //printf("string width adjust dx=%d glyph_width=%d x-offset=%d\n", dx, u8g2->font_decode.glyph_width, u8g2->glyph_x_offset);
    w -= dx;
    w += u8g2->font_decode.glyph_width;  /* the real pixel width of the glyph, sideeffect of GetGlyphWidth */
    /* issue #46: we have to add the x offset also */
    w += u8g2->glyph_x_offset;	/* this value is set as a side effect of u8g2_GetGlyphWidth() */
#ifdef U8G2_BALANCED_STR_WIDTH_CALCULATION
    /* https://github.com/olikraus/u8g2/issues/1561 */
    // issue #1561：首字符有正 x_offset（左侧留白）时补回，宽度更"均衡"
    if ( initial_x_offset > 0 )
      w+=initial_x_offset;
#endif 
  }
  // printf("w=%d \n", w);
  
  return w;  
}

/**
 * @brief   返回指定字形相对基准点的 X 偏移（bbox 的 ox 字段）
 * @note    复用 u8g2_GetGlyphWidth 并取其副作用 glyph_x_offset
 */
int8_t u8g2_GetXOffsetGlyph(u8g2_t *u8g2, uint16_t encoding)
{
  u8g2_GetGlyphWidth(u8g2, encoding);		/* delta x value of the glyph, side effect: updates u8g2->glyph_x_offset */
  return u8g2->glyph_x_offset;
}

/**
 * @brief   返回 UTF-8 串首个码点的 X 偏移（解码到第一个有效码点即停）
 * @return  首码点 x_offset；串为空/无有效码点返回 0
 */
int8_t u8g2_GetXOffsetUTF8(u8g2_t *u8g2, const char *utf8)
{
  uint16_t e; 
  u8x8_utf8_init(u8g2_GetU8x8(u8g2));
  for(;;)  // extract encoding from UTF8 byte stream
  {
    e = u8x8_utf8_next(u8g2_GetU8x8(u8g2), (uint8_t)*utf8);
    if ( e == 0x0ffff )
      return 0;
    if ( e < 0x0fffe )  // 0x0fffe means: just continue 
      break;
    utf8++;
  }
  return u8g2_GetXOffsetGlyph(u8g2, e);
}

/**
 * @brief   一次取出字形的三个水平度量（旧 u8g 兼容层/精确宽度算法使用）
 * @param   w  输出：字形位图宽 glyph_width
 * @param   ox 输出：X 偏移（有符号）
 * @param   dx 输出：步进宽度
 * @note    字形不存在时不写任何输出（保持调用者初值）；本质是"只解头不解位图"的轻量查询
 */
static void u8g2_GetGlyphHorizontalProperties(u8g2_t *u8g2, uint16_t requested_encoding, uint8_t *w, int8_t *ox, int8_t *dx)
{
  const uint8_t *glyph_data = u8g2_font_get_glyph_data(u8g2, requested_encoding);
  if ( glyph_data == NULL )
    return; 
  
  u8g2_font_setup_decode(u8g2, glyph_data);
  *w = u8g2->font_decode.glyph_width;
  *ox =  u8g2_font_decode_get_signed_bits(&(u8g2->font_decode), u8g2->font_info.bits_per_char_x);
  u8g2_font_decode_get_signed_bits(&(u8g2->font_decode), u8g2->font_info.bits_per_char_y);
  *dx = u8g2_font_decode_get_signed_bits(&(u8g2->font_decode), u8g2->font_info.bits_per_delta_x);
}

/* u8g compatible GetStrX function */
/**
 * @brief   u8g 兼容接口：返回字符串首字符的 X 偏移
 * @note    只看首字符（*s），不做 UTF-8 解码
 */
int8_t u8g2_GetStrX(u8g2_t *u8g2, const char *s)
{
  uint8_t w;
  int8_t dx;
  int8_t ox = 0;
  u8g2_GetGlyphHorizontalProperties(u8g2, *s, &w, &ox, &dx);
  return ox;
}


/*
Warning: This function needs to be fixed. I think it was taken over from u8glib, but not fixed as of now 
The main difference for this procedure compared to the normal get width, should be, that the initial
offset is removed

Idea: for the user interface it probably would be better to add the xoffset of the first char to the end, so that the overall word looks better.
Maybe then the procedure should be called differently, maybe balanced width instead of exact width

u8g2_calculate_exact_string_width is now OBSOLETE, instead the above str width calculation has been updated:
https://github.com/olikraus/u8g2/issues/1561
*/
#ifdef OBSOLETE
static u8g2_uint_t u8g2_calculate_exact_string_width(u8g2_t *u8g2, const char *str)
{
  const char *s = str;
  uint16_t enc;
  u8g2_uint_t  w;
  uint8_t cnt;
  uint8_t gw; 
  int8_t ox, dx;
  
  /* reset the total minimal width to zero, this will be expanded during calculation */
  w = 0;
    
  
  /* check for empty string, width is already 0 */
  cnt = 0;
  
  for(;;)
  {
    enc = u8g2->u8x8.next_cb(u8g2_GetU8x8(u8g2), (uint8_t)*s);
    if ( enc == 0x0ffff )
      break;    
    s++;
    if ( enc != 0x0fffe )
    {
      if ( cnt == 0 )
      {
        /* get glyph properties of the first char */
        u8g2_GetGlyphHorizontalProperties(u8g2, enc, &gw, &ox, &dx);  
      }        
      cnt++;
      if ( cnt > 2 )
        break;
    }
  }
  
  if ( cnt == 0 )
    return 0;
   
  if ( cnt == 1 )
     return gw;

  /* strlen(s) == 1:       width = width(s[0]) */
  /* strlen(s) == 2:       width = - offx(s[0]) + deltax(s[0]) + offx(s[1]) + width(s[1]) */
  /* strlen(s) == 3:       width = - offx(s[0]) + deltax(s[0]) + deltax(s[1]) + offx(s[2]) + width(s[2]) */
  
  /* assume that the string has size 2 or more, than start with negative offset-x */
  /* for string with size 1, this will be nullified after the loop */
  w = -ox;  
  for(;;)
  {
    enc = u8g2->u8x8.next_cb(u8g2_GetU8x8(u8g2), (uint8_t)*str);
    if ( enc== 0x0ffff )
      break;
    str++;
    if ( enc != 0x0fffe )
    {
      u8g2_GetGlyphHorizontalProperties(u8g2, enc, &gw, &ox, &dx);        
      /* if there are still more characters, add the delta to the next glyph */
      w += dx;    
    }
  }
  
  /* finally calculate the width of the last char */
  /* here is another exception, if the last char is a blank, use the dx value instead */
  if ( gw != 0 )
  {
    w -= dx;    /* remove the last dx */
    /* if g was not updated in the for loop (strlen() == 1), then the initial offset x gets removed */
    w += gw;
    w += ox;
  }
  else
  {
    //w += dx;
  }
  
  
  return w;
	
}
#endif





/**
 * @brief   计算 ASCII 字符串宽度（挂 ascii 回调后进 u8g2_string_width）
 * @return  宽度（像素）；菜单/对话框居中排版最常用的测量接口
 */
u8g2_uint_t u8g2_GetStrWidth(u8g2_t *u8g2, const char *s)
{
  u8g2->u8x8.next_cb = u8x8_ascii_next;
  return u8g2_string_width(u8g2, s);
}

/* OBSOLETE
u8g2_uint_t u8g2_GetExactStrWidth(u8g2_t *u8g2, const char *s)
{
  u8g2->u8x8.next_cb = u8x8_ascii_next;
  return u8g2_calculate_exact_string_width(u8g2, s);
}
*/

/*
source: https://en.wikipedia.org/wiki/UTF-8
Bits	from 		to			bytes	Byte 1 		Byte 2 		Byte 3 		Byte 4 		Byte 5 		Byte 6
  7 	U+0000 		U+007F 		1 		0xxxxxxx
11 	U+0080 		U+07FF 		2 		110xxxxx 	10xxxxxx
16 	U+0800 		U+FFFF 		3 		1110xxxx 	10xxxxxx 	10xxxxxx
21 	U+10000 	U+1FFFFF 	4 		11110xxx 	10xxxxxx 	10xxxxxx 	10xxxxxx
26 	U+200000 	U+3FFFFFF 	5 		111110xx 	10xxxxxx 	10xxxxxx 	10xxxxxx 	10xxxxxx
31 	U+4000000 	U+7FFFFFFF 	6 		1111110x 	10xxxxxx 	10xxxxxx 	10xxxxxx 	10xxxxxx 	10xxxxxx  
*/
/**
 * @brief   计算 UTF-8 字符串宽度（挂 utf8 回调后进 u8g2_string_width）
 */
u8g2_uint_t u8g2_GetUTF8Width(u8g2_t *u8g2, const char *str)
{
  u8g2->u8x8.next_cb = u8x8_utf8_next;
  return u8g2_string_width(u8g2, str);
}



/**
 * @brief   设置字符串书写方向 0/1/2/3 = 0°/90°/180°/270°（需 U8G2_WITH_FONT_ROTATION）
 * @note    方向存进 font_decode.dir，影响 DrawGlyph 的参考点换算、decode_len 的
 *          旋转映射与 draw_string 的光标推进；纯标志位，无立即开销
 */
void u8g2_SetFontDirection(u8g2_t *u8g2, uint8_t dir)
{
#ifdef U8G2_WITH_FONT_ROTATION  
  u8g2->font_decode.dir = dir;
#endif
}

/*=======================================*/
/*
  Draw a string, which is produced by hbshape2u8g2 (libharfbuzz toolchain)

  The data argument should look like this:
    static const unsigned char teststring[] U8X8_PROGMEM = {
      0x09, 0x28, 0x00, 0x00, // u8g2_DrawGlyph(&u8g2, 0, 0, 2344);
      0x09, 0x2e, 0x10, 0x00, // u8g2_DrawGlyph(&u8g2, 16, 0, 2350);
      0x09, 0x38, 0x10, 0x00, // u8g2_DrawGlyph(&u8g2, 32, 0, 2360);
      0x09, 0x4d, 0x00, 0x00, // u8g2_DrawGlyph(&u8g2, 32, 0, 2381);
      0x09, 0x24, 0x10, 0x00, // u8g2_DrawGlyph(&u8g2, 48, 0, 2340);
      0x09, 0x47, 0x00, 0x00, // u8g2_DrawGlyph(&u8g2, 48, 0, 2375);
      0x00, 0x00  // end of binary
    };

  The data row contains four bytes, which are:
    <encoding high byte> <encoding low byte> <delta-x> <delta-y>
  The last row is marked with encoding=0

  The algorithm is
    Input: x,y
    with all rows (until encoding is 0)      
      x += <delta-x>
      y += <delta-y>
      draw glyph with <encoding> at x, y

  A call to this function will require transparent mode:
    u8g2_SetFontMode(&u8g2, 1);
    
  Limitation:
    Glyph delta must be lower than 128, this bascially means, that the glyph size is limited to
    hight/width of 128 pixel
    
  Further details: 
    https://github.com/olikraus/u8g2/issues/2656

*/
/**
 * @brief   绘制 hbshape2u8g2（HarfBuzz 整形工具链）输出的"字形序列"二进制数据
 * @param   x,y  起始基准点
 * @param   data 行式数据，每行 4 字节：[编码高字节][编码低字节][delta-x][delta-y]，
 *               以编码=0 的行结尾
 * @note    用于阿拉伯语等复杂文字：选形（contextual shaping）与定位已在 PC 端由
 *          HarfBuzz 完成，MCU 端只按表逐个平移绘制，是"离屏整形、在线回放"模式。
 *          调用前必须 u8g2_SetFontMode(&u8g2, 1)（透明模式）；每个 delta 需 < 128
 *          （int8_t 有符号单字节），即字形尺寸上限约 128 像素（issue #2656）
 */
void u8g2_DrawHB(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, const unsigned char *data)
{
    uint16_t encoding = 0;
    for (;;)
    {
        encoding = u8x8_pgm_read(data);
        data++;
        encoding <<= 8;
        encoding  |= u8x8_pgm_read(data);
        data++;
        if (encoding == 0)
            break;
        // delta 按 int8_t 解释：负值即向左/向上回退（RTL 场景）
        x += (int8_t)u8x8_pgm_read(data);
        data++;
        y += (int8_t)u8x8_pgm_read(data);
        data++;
        u8g2_DrawGlyph(u8g2, x, y, encoding);
    }
}

