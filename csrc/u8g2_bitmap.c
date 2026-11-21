/*
 * ============================================================================
 * 【中文导读】u8g2_bitmap.c —— 位图绘制（Bitmap / XBM / XBMP 三格式）
 * ----------------------------------------------------------------------------
 * 职责：6 个公共入口 = 3 种数据格式 × "整幅 / 单行"两层：
 *   u8g2_DrawBitmap         整幅；行格式 MSB-first（mask 从 0x80 起），cnt 为每行字节数
 *   u8g2_DrawXBM            整幅；行格式 LSB-first（mask 从 0x01 起），每行 (w+7)/8 字节
 *   u8g2_DrawXBMP           同 XBM，但数据在 AVR PROGMEM（读像素经 u8x8_pgm_read）
 *   u8g2_DrawHorizontalBitmap / DrawHXBM / DrawHXBMP   对应的单行底层
 * 透明模式：u8g2_SetBitmapMode(1) 时 0 位不画（保背景），0 时画反色 ncolor。
 * 关键优化：HXBM/HXBMP 内循环先测"同色游程长度"，再一次性调
 *   u8g2_DrawHVLine 画一整段——比 OLD 宏保留的逐像素版本少一个数量级调用开销。
 * 位置：应用层 → 本文件 → u8g2_DrawHVLine → u8g2_ll_hvline → 显示缓冲；
 * 整幅入口均先 u8g2_IsIntersection 剪裁，出屏直接返回。
 * ============================================================================
 */

/*

  u8g2_bitmap.c

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


void u8g2_SetBitmapMode(u8g2_t *u8g2, uint8_t is_transparent) {
  u8g2->bitmap_transparency = is_transparent;
}

/*
  x,y 	Position on the display
  len		Length of bitmap line in pixel. Note: This differs from u8glib which had a bytecount here.
  b		Pointer to the bitmap line.
  Only draw pixels which are set.
*/

void u8g2_DrawHorizontalBitmap(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t len, const uint8_t *b)
{
  uint8_t mask;
  uint8_t color = u8g2->draw_color;
  uint8_t ncolor = (color == 0 ? 1 : 0);

#ifdef U8G2_WITH_INTERSECTION
  if ( u8g2_IsIntersection(u8g2, x, y, x+len, y+1) == 0 ) 
    return;
#endif /* U8G2_WITH_INTERSECTION */
  
  mask = 128;
  while(len > 0)
  {
    if ( *b & mask ) {
      u8g2->draw_color = color;
      u8g2_DrawHVLine(u8g2, x, y, 1, 0);
    } else if ( u8g2->bitmap_transparency == 0 ) {
      u8g2->draw_color = ncolor;
      u8g2_DrawHVLine(u8g2, x, y, 1, 0);
    }

    x++;
    mask >>= 1;
    if ( mask == 0 )
    {
      mask = 128;
      b++;
    }
    len--;
  }
  u8g2->draw_color = color;
}


/* u8glib compatible bitmap draw function */
void u8g2_DrawBitmap(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t cnt, u8g2_uint_t h, const uint8_t *bitmap)
{
  u8g2_uint_t w;
  w = cnt;
  w *= 8;
#ifdef U8G2_WITH_INTERSECTION
  if ( u8g2_IsIntersection(u8g2, x, y, x+w, y+h) == 0 ) 
    return;
#endif /* U8G2_WITH_INTERSECTION */
  
  while( h > 0 )
  {
    u8g2_DrawHorizontalBitmap(u8g2, x, y, w, bitmap);
    bitmap += cnt;
    y++;
    h--;
  }
}



void u8g2_DrawHXBM(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t len, const uint8_t *b)
{
  uint8_t mask;
  uint8_t color = u8g2->draw_color;
  uint8_t ncolor = (color == 0 ? 1 : 0);
#ifdef U8G2_WITH_INTERSECTION
  if ( u8g2_IsIntersection(u8g2, x, y, x+len, y+1) == 0 ) 
    return;
#endif /* U8G2_WITH_INTERSECTION */
  
  mask = 1;
  while(len > 0) {
    uint8_t current_bit = (*b) & mask;
#ifdef OLD
    if ( current_bit ) {
      u8g2->draw_color = color;
      u8g2_DrawHVLine(u8g2, x, y, 1, 0);
    } else if ( u8g2->bitmap_transparency == 0 ) {
      u8g2->draw_color = ncolor;
      u8g2_DrawHVLine(u8g2, x, y, 1, 0);
    }
    x++;
    mask <<= 1;
    if ( mask == 0 )
    {
      mask = 1;
      b++;
    }
    len--;
#else
    u8g2_uint_t run_length = 0;
    // Determine the run length of consecutive bits with the same color
    while (len > 0 && (current_bit == 0 ? (((*b) & mask) == 0) : (((*b) & mask) != 0 )  ))
    {
        run_length++;
        x++;
        mask <<= 1;
        if (mask == 0)
        {
            mask = 1;
            b++;
        }
        len--;
    }
    if (current_bit)
    {
        u8g2->draw_color = color;
        u8g2_DrawHVLine(u8g2, x - run_length, y, run_length, 0);
    }
    else if (u8g2->bitmap_transparency == 0)
    {
        u8g2->draw_color = ncolor;
        u8g2_DrawHVLine(u8g2, x - run_length, y, run_length, 0);
    }
#endif
  }
  u8g2->draw_color = color;
}


void u8g2_DrawXBM(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t w, u8g2_uint_t h, const uint8_t *bitmap)
{
  u8g2_uint_t blen;
  blen = w;
  blen += 7;
  blen >>= 3;
#ifdef U8G2_WITH_INTERSECTION
  if ( u8g2_IsIntersection(u8g2, x, y, x+w, y+h) == 0 ) 
    return;
#endif /* U8G2_WITH_INTERSECTION */
  
  while( h > 0 )
  {
    u8g2_DrawHXBM(u8g2, x, y, w, bitmap);
    bitmap += blen;
    y++;
    h--;
  }
}






void u8g2_DrawHXBMP(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t len, const uint8_t *b)
{
  uint8_t mask;
  uint8_t color = u8g2->draw_color;
  uint8_t ncolor = (color == 0 ? 1 : 0);
#ifdef U8G2_WITH_INTERSECTION
  if ( u8g2_IsIntersection(u8g2, x, y, x+len, y+1) == 0 ) 
    return;
#endif /* U8G2_WITH_INTERSECTION */
  
  mask = 1;
  while(len > 0)
  {
    uint8_t current_bit = u8x8_pgm_read(b) & mask;
//#define OLD
#ifdef OLD
    if ( current_bit ) {
      u8g2->draw_color = color;
      u8g2_DrawHVLine(u8g2, x, y, 1, 0);
    } else if( u8g2->bitmap_transparency == 0 ) {
      u8g2->draw_color = ncolor;
      u8g2_DrawHVLine(u8g2, x, y, 1, 0);
    }
   
    x++;
    mask <<= 1;
    if ( mask == 0 )
    {
      mask = 1;
      b++;
    }
    len--;
#else
    u8g2_uint_t run_length = 0;
    // Determine the run length of consecutive bits with the same color
    while (len > 0 && (current_bit == 0 ? ((u8x8_pgm_read(b) & mask) == 0) : ((u8x8_pgm_read(b) & mask) != 0 )  ))
    {
        run_length++;
        x++;
        mask <<= 1;
        if (mask == 0)
        {
            mask = 1;
            b++;
        }
        len--;
    }
    if (current_bit)
    {
        u8g2->draw_color = color;
        u8g2_DrawHVLine(u8g2, x - run_length, y, run_length, 0);
    }
    else if (u8g2->bitmap_transparency == 0)
    {
        u8g2->draw_color = ncolor;
        u8g2_DrawHVLine(u8g2, x - run_length, y, run_length, 0);
    }
#endif
  }
  u8g2->draw_color = color;
}


void u8g2_DrawXBMP(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t w, u8g2_uint_t h, const uint8_t *bitmap)
{
  u8g2_uint_t blen;
  blen = w;
  blen += 7;
  blen >>= 3;
#ifdef U8G2_WITH_INTERSECTION
  if ( u8g2_IsIntersection(u8g2, x, y, x+w, y+h) == 0 ) 
    return;
#endif /* U8G2_WITH_INTERSECTION */
  
  while( h > 0 )
  {
    u8g2_DrawHXBMP(u8g2, x, y, w, bitmap);
    bitmap += blen;
    y++;
    h--;
  }
}


