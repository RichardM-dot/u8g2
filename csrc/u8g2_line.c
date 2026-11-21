/*
 * ============================================================================
 * 【中文导读】u8g2_line.c —— 直线图元（整数 Bresenham）
 * ----------------------------------------------------------------------------
 * 职责：唯一公共入口 u8g2_DrawLine(x1,y1,x2,y2)，标准 Bresenham 算法三步：
 *   1. dx/dy 取绝对值；若 |dy|>|dx| 置 swapxy 并交换 x/y（斜率>1 时改为按 x 步进）；
 *   2. 若 x1>x2 再交换两端点，保证从左往右步进；ystep 由 y2>=y1 决定 ±1；
 *   3. err = dx/2 起步，每步 err -= dy，err<0 时 y += ystep 并 err += dx（误差累计）。
 * 全程整数加减，无乘除、无浮点。
 * 注意：本文件不做窗口剪裁（原注释 "no intersection check"），越界交给
 *   u8g2_DrawPixel 内部逐点兜底，长斜线开销略高；水平/垂直线请走
 *   u8g2_DrawHLine/DrawVLine（u8g2_hvline.c）的快速路径。
 * 细节：终点 x2 == 255（8bit 模式）或 0xffff（16bit 模式）时先减 1，防止
 *   for 循环计数回绕成死循环——这是 u8g2_uint_t 无符号回绕的经典防御。
 * 位置：应用层 → 本文件 → u8g2_DrawPixel → u8g2_ll_hvline → 显示缓冲。
 * （保留上方上游文件的版权头：其文件名误写为 u8g2_box.c，属 u8g2 上游笔误）
 * ============================================================================
 */

/*

  u8g2_box.c

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


void u8g2_DrawLine(u8g2_t *u8g2, u8g2_uint_t x1, u8g2_uint_t y1, u8g2_uint_t x2, u8g2_uint_t y2)
{
  u8g2_uint_t tmp;
  u8g2_uint_t x,y;
  u8g2_uint_t dx, dy;
  u8g2_int_t err;
  u8g2_int_t ystep;

  uint8_t swapxy = 0;
  
  /* no intersection check at the moment, should be added... */

  if ( x1 > x2 ) dx = x1-x2; else dx = x2-x1;
  if ( y1 > y2 ) dy = y1-y2; else dy = y2-y1;

  if ( dy > dx ) 
  {
    swapxy = 1;
    tmp = dx; dx =dy; dy = tmp;
    tmp = x1; x1 =y1; y1 = tmp;
    tmp = x2; x2 =y2; y2 = tmp;
  }
  if ( x1 > x2 ) 
  {
    tmp = x1; x1 =x2; x2 = tmp;
    tmp = y1; y1 =y2; y2 = tmp;
  }
  err = dx >> 1;
  if ( y2 > y1 ) ystep = 1; else ystep = -1;
  y = y1;

#ifndef  U8G2_16BIT
  if ( x2 == 255 )
    x2--;
#else
  if ( x2 == 0xffff )
    x2--;
#endif

  for( x = x1; x <= x2; x++ )
  {
    if ( swapxy == 0 ) 
      u8g2_DrawPixel(u8g2, x, y); 
    else 
      u8g2_DrawPixel(u8g2, y, x); 
    err -= (u8g2_uint_t)dy;
    if ( err < 0 ) 
    {
      y += (u8g2_uint_t)ystep;
      err += (u8g2_uint_t)dx;
    }
  }
}

