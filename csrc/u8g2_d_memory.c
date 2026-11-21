/*
 * ============================================================================
 * 【中文导读·B级】u8g2_d_memory.c —— 静态帧缓冲分配表（生成代码，勿手工编辑）
 * ----------------------------------------------------------------------------
 * 职责：为"每一种屏 × 每一种内存模式"提供一个缓冲分配函数。全文件共 225 个
 * u8g2_m_<tile宽>_<tile高>_<模式> 函数，全部由 tools/codebuild 生成。
 *   - 模式后缀 1 = 单页缓冲（tile_buf_height=1，页模式最省 RAM）
 *   - 模式后缀 2 = 两页缓冲（tile_buf_height=2，页模式折中）
 *   - 模式后缀 f = 全缓冲（tile_buf_height=满屏行数，如 128x64 → 1024B）
 * 每个函数体都是同一个模板：#ifdef U8G2_USE_DYNAMIC_ALLOC 时返回 0（由用户
 * 用 u8g2_SetBufferPtr 提供缓冲），否则返回 static uint8_t buf[N] 并写回 *page_cnt。
 * 真实大小示例：u8g2_m_16_8_1 → 16 tile 宽 × 8 字节 × 1 页 = 128B；
 *               u8g2_m_16_8_f → 16 × 8 × 8 = 1024B。
 * 在数据流中的位置：u8g2_d_setup.c 的每个 Setup 函数调用这里拿 buf 指针，
 * 随后交给 u8g2_SetupBuffer() 装配。链接器只保留被实际引用的数组。
 * 深入阅读：Docs/01-内存与缓冲模型.md；声明见 u8g2.h"生成代码声明区 1/2"。
 * ============================================================================
 */
/* u8g2_d_memory.c */
/* generated code, codebuild, u8g2 project */

#include "u8g2.h"

uint8_t *u8g2_m_16_4_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[128];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_16_4_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[256];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_16_4_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 4;
  return 0;
  #else
  static uint8_t buf[512];
  *page_cnt = 4;
  return buf;
  #endif
}
uint8_t *u8g2_m_16_8_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[128];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_16_8_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[256];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_16_8_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 8;
  return 0;
  #else
  static uint8_t buf[1024];
  *page_cnt = 8;
  return buf;
  #endif
}
uint8_t *u8g2_m_255_2_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[2040];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_255_2_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[4080];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_255_2_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[4080];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_9_5_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[72];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_9_5_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[144];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_9_5_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 5;
  return 0;
  #else
  static uint8_t buf[360];
  *page_cnt = 5;
  return buf;
  #endif
}
uint8_t *u8g2_m_12_5_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[96];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_12_5_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[192];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_12_5_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 5;
  return 0;
  #else
  static uint8_t buf[480];
  *page_cnt = 5;
  return buf;
  #endif
}
uint8_t *u8g2_m_8_4_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[64];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_8_4_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[128];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_8_4_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 4;
  return 0;
  #else
  static uint8_t buf[256];
  *page_cnt = 4;
  return buf;
  #endif
}
uint8_t *u8g2_m_8_16_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[64];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_8_16_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[128];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_8_16_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 16;
  return 0;
  #else
  static uint8_t buf[1024];
  *page_cnt = 16;
  return buf;
  #endif
}
uint8_t *u8g2_m_12_12_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[96];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_12_12_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[192];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_12_12_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 12;
  return 0;
  #else
  static uint8_t buf[1152];
  *page_cnt = 12;
  return buf;
  #endif
}
uint8_t *u8g2_m_10_16_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[80];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_10_16_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[160];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_10_16_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 16;
  return 0;
  #else
  static uint8_t buf[1280];
  *page_cnt = 16;
  return buf;
  #endif
}
uint8_t *u8g2_m_16_16_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[128];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_16_16_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[256];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_16_16_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 16;
  return 0;
  #else
  static uint8_t buf[2048];
  *page_cnt = 16;
  return buf;
  #endif
}
uint8_t *u8g2_m_16_20_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[128];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_16_20_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[256];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_16_20_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 20;
  return 0;
  #else
  static uint8_t buf[2560];
  *page_cnt = 20;
  return buf;
  #endif
}
uint8_t *u8g2_m_20_20_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[160];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_20_20_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[320];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_20_20_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 20;
  return 0;
  #else
  static uint8_t buf[3200];
  *page_cnt = 20;
  return buf;
  #endif
}
uint8_t *u8g2_m_32_8_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[256];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_32_8_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[512];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_32_8_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 8;
  return 0;
  #else
  static uint8_t buf[2048];
  *page_cnt = 8;
  return buf;
  #endif
}
uint8_t *u8g2_m_13_8_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[104];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_13_8_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[208];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_13_8_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 8;
  return 0;
  #else
  static uint8_t buf[832];
  *page_cnt = 8;
  return buf;
  #endif
}
uint8_t *u8g2_m_8_6_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[64];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_8_6_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[128];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_8_6_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 6;
  return 0;
  #else
  static uint8_t buf[384];
  *page_cnt = 6;
  return buf;
  #endif
}
uint8_t *u8g2_m_11_6_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[88];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_11_6_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[176];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_11_6_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 6;
  return 0;
  #else
  static uint8_t buf[528];
  *page_cnt = 6;
  return buf;
  #endif
}
uint8_t *u8g2_m_6_8_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[48];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_6_8_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[96];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_6_8_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 8;
  return 0;
  #else
  static uint8_t buf[384];
  *page_cnt = 8;
  return buf;
  #endif
}
uint8_t *u8g2_m_12_2_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[96];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_12_2_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[192];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_12_2_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[192];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_15_4_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[120];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_15_4_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[240];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_15_4_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 4;
  return 0;
  #else
  static uint8_t buf[480];
  *page_cnt = 4;
  return buf;
  #endif
}
uint8_t *u8g2_m_12_4_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[96];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_12_4_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[192];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_12_4_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 4;
  return 0;
  #else
  static uint8_t buf[384];
  *page_cnt = 4;
  return buf;
  #endif
}
uint8_t *u8g2_m_16_12_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[128];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_16_12_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[256];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_16_12_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 12;
  return 0;
  #else
  static uint8_t buf[1536];
  *page_cnt = 12;
  return buf;
  #endif
}
uint8_t *u8g2_m_32_4_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[256];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_32_4_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[512];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_32_4_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 4;
  return 0;
  #else
  static uint8_t buf[1024];
  *page_cnt = 4;
  return buf;
  #endif
}
uint8_t *u8g2_m_12_8_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[96];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_12_8_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[192];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_12_8_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 8;
  return 0;
  #else
  static uint8_t buf[768];
  *page_cnt = 8;
  return buf;
  #endif
}
uint8_t *u8g2_m_16_5_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[128];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_16_5_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[256];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_16_5_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 5;
  return 0;
  #else
  static uint8_t buf[640];
  *page_cnt = 5;
  return buf;
  #endif
}
uint8_t *u8g2_m_18_4_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[144];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_18_4_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[288];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_18_4_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 4;
  return 0;
  #else
  static uint8_t buf[576];
  *page_cnt = 4;
  return buf;
  #endif
}
uint8_t *u8g2_m_20_4_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[160];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_20_4_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[320];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_20_4_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 4;
  return 0;
  #else
  static uint8_t buf[640];
  *page_cnt = 4;
  return buf;
  #endif
}
uint8_t *u8g2_m_24_4_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[192];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_24_4_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[384];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_24_4_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 4;
  return 0;
  #else
  static uint8_t buf[768];
  *page_cnt = 4;
  return buf;
  #endif
}
uint8_t *u8g2_m_50_30_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[400];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_50_30_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[800];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_50_30_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 30;
  return 0;
  #else
  static uint8_t buf[12000];
  *page_cnt = 30;
  return buf;
  #endif
}
uint8_t *u8g2_m_18_21_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[144];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_18_21_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[288];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_18_21_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 21;
  return 0;
  #else
  static uint8_t buf[3024];
  *page_cnt = 21;
  return buf;
  #endif
}
uint8_t *u8g2_m_20_9_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[160];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_20_9_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[320];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_20_9_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 9;
  return 0;
  #else
  static uint8_t buf[1440];
  *page_cnt = 9;
  return buf;
  #endif
}
uint8_t *u8g2_m_12_9_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[96];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_12_9_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[192];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_12_9_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 9;
  return 0;
  #else
  static uint8_t buf[864];
  *page_cnt = 9;
  return buf;
  #endif
}
uint8_t *u8g2_m_24_8_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[192];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_24_8_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[384];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_24_8_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 8;
  return 0;
  #else
  static uint8_t buf[1536];
  *page_cnt = 8;
  return buf;
  #endif
}
uint8_t *u8g2_m_30_8_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[240];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_30_8_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[480];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_30_8_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 8;
  return 0;
  #else
  static uint8_t buf[1920];
  *page_cnt = 8;
  return buf;
  #endif
}
uint8_t *u8g2_m_30_15_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[240];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_30_15_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[480];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_30_15_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 15;
  return 0;
  #else
  static uint8_t buf[3600];
  *page_cnt = 15;
  return buf;
  #endif
}
uint8_t *u8g2_m_30_16_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[240];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_30_16_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[480];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_30_16_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 16;
  return 0;
  #else
  static uint8_t buf[3840];
  *page_cnt = 16;
  return buf;
  #endif
}
uint8_t *u8g2_m_32_16_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[256];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_32_16_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[512];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_32_16_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 16;
  return 0;
  #else
  static uint8_t buf[4096];
  *page_cnt = 16;
  return buf;
  #endif
}
uint8_t *u8g2_m_20_16_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[160];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_20_16_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[320];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_20_16_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 16;
  return 0;
  #else
  static uint8_t buf[2560];
  *page_cnt = 16;
  return buf;
  #endif
}
uint8_t *u8g2_m_24_12_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[192];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_24_12_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[384];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_24_12_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 12;
  return 0;
  #else
  static uint8_t buf[2304];
  *page_cnt = 12;
  return buf;
  #endif
}
uint8_t *u8g2_m_20_13_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[160];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_20_13_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[320];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_20_13_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 13;
  return 0;
  #else
  static uint8_t buf[2080];
  *page_cnt = 13;
  return buf;
  #endif
}
uint8_t *u8g2_m_30_20_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[240];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_30_20_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[480];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_30_20_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 20;
  return 0;
  #else
  static uint8_t buf[4800];
  *page_cnt = 20;
  return buf;
  #endif
}
uint8_t *u8g2_m_40_30_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[320];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_40_30_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[640];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_40_30_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 30;
  return 0;
  #else
  static uint8_t buf[9600];
  *page_cnt = 30;
  return buf;
  #endif
}
uint8_t *u8g2_m_80_40_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[640];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_80_40_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[1280];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_80_40_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 40;
  return 0;
  #else
  static uint8_t buf[25600];
  *page_cnt = 40;
  return buf;
  #endif
}
uint8_t *u8g2_m_20_8_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[160];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_20_8_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[320];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_20_8_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 8;
  return 0;
  #else
  static uint8_t buf[1280];
  *page_cnt = 8;
  return buf;
  #endif
}
uint8_t *u8g2_m_17_4_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[136];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_17_4_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[272];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_17_4_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 4;
  return 0;
  #else
  static uint8_t buf[544];
  *page_cnt = 4;
  return buf;
  #endif
}
uint8_t *u8g2_m_17_8_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[136];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_17_8_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[272];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_17_8_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 8;
  return 0;
  #else
  static uint8_t buf[1088];
  *page_cnt = 8;
  return buf;
  #endif
}
uint8_t *u8g2_m_17_9_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[136];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_17_9_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[272];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_17_9_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 9;
  return 0;
  #else
  static uint8_t buf[1224];
  *page_cnt = 9;
  return buf;
  #endif
}
uint8_t *u8g2_m_16_32_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[128];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_16_32_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[256];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_16_32_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 32;
  return 0;
  #else
  static uint8_t buf[4096];
  *page_cnt = 32;
  return buf;
  #endif
}
uint8_t *u8g2_m_26_25_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[208];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_26_25_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[416];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_26_25_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 25;
  return 0;
  #else
  static uint8_t buf[5200];
  *page_cnt = 25;
  return buf;
  #endif
}
uint8_t *u8g2_m_21_48_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[168];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_21_48_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[336];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_21_48_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 48;
  return 0;
  #else
  static uint8_t buf[8064];
  *page_cnt = 48;
  return buf;
  #endif
}
uint8_t *u8g2_m_38_50_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[304];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_38_50_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[608];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_38_50_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 50;
  return 0;
  #else
  static uint8_t buf[15200];
  *page_cnt = 50;
  return buf;
  #endif
}
uint8_t *u8g2_m_48_17_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[384];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_48_17_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[768];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_48_17_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 17;
  return 0;
  #else
  static uint8_t buf[6528];
  *page_cnt = 17;
  return buf;
  #endif
}
uint8_t *u8g2_m_48_20_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[384];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_48_20_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[768];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_48_20_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 20;
  return 0;
  #else
  static uint8_t buf[7680];
  *page_cnt = 20;
  return buf;
  #endif
}
uint8_t *u8g2_m_42_20_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[336];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_42_20_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[672];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_42_20_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 20;
  return 0;
  #else
  static uint8_t buf[6720];
  *page_cnt = 20;
  return buf;
  #endif
}
uint8_t *u8g2_m_20_12_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[160];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_20_12_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[320];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_20_12_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 12;
  return 0;
  #else
  static uint8_t buf[1920];
  *page_cnt = 12;
  return buf;
  #endif
}
uint8_t *u8g2_m_32_20_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[256];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_32_20_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[512];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_32_20_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 20;
  return 0;
  #else
  static uint8_t buf[5120];
  *page_cnt = 20;
  return buf;
  #endif
}
uint8_t *u8g2_m_22_13_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[176];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_22_13_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[352];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_22_13_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 13;
  return 0;
  #else
  static uint8_t buf[2288];
  *page_cnt = 13;
  return buf;
  #endif
}
uint8_t *u8g2_m_20_10_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[160];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_20_10_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[320];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_20_10_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 10;
  return 0;
  #else
  static uint8_t buf[1600];
  *page_cnt = 10;
  return buf;
  #endif
}
uint8_t *u8g2_m_80_4_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[640];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_80_4_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[1280];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_80_4_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 4;
  return 0;
  #else
  static uint8_t buf[2560];
  *page_cnt = 4;
  return buf;
  #endif
}
uint8_t *u8g2_m_19_4_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[152];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_19_4_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[304];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_19_4_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 4;
  return 0;
  #else
  static uint8_t buf[608];
  *page_cnt = 4;
  return buf;
  #endif
}
uint8_t *u8g2_m_16_9_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[128];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_16_9_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[256];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_16_9_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 9;
  return 0;
  #else
  static uint8_t buf[1152];
  *page_cnt = 9;
  return buf;
  #endif
}
uint8_t *u8g2_m_20_17_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[160];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_20_17_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[320];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_20_17_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 17;
  return 0;
  #else
  static uint8_t buf[2720];
  *page_cnt = 17;
  return buf;
  #endif
}
uint8_t *u8g2_m_26_5_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[208];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_26_5_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[416];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_26_5_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 5;
  return 0;
  #else
  static uint8_t buf[1040];
  *page_cnt = 5;
  return buf;
  #endif
}
uint8_t *u8g2_m_22_9_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[176];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_22_9_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[352];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_22_9_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 9;
  return 0;
  #else
  static uint8_t buf[1584];
  *page_cnt = 9;
  return buf;
  #endif
}
uint8_t *u8g2_m_25_25_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[200];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_25_25_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[400];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_25_25_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 25;
  return 0;
  #else
  static uint8_t buf[5000];
  *page_cnt = 25;
  return buf;
  #endif
}
uint8_t *u8g2_m_37_16_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[296];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_37_16_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[592];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_37_16_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 16;
  return 0;
  #else
  static uint8_t buf[4736];
  *page_cnt = 16;
  return buf;
  #endif
}
uint8_t *u8g2_m_40_25_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[320];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_40_25_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[640];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_40_25_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 25;
  return 0;
  #else
  static uint8_t buf[8000];
  *page_cnt = 25;
  return buf;
  #endif
}
uint8_t *u8g2_m_40_20_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[320];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_40_20_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[640];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_40_20_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 20;
  return 0;
  #else
  static uint8_t buf[6400];
  *page_cnt = 20;
  return buf;
  #endif
}
uint8_t *u8g2_m_8_1_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[64];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_8_1_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[128];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_8_1_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[64];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_4_1_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[32];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_4_1_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[64];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_4_1_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[32];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_1_1_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[8];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_1_1_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[16];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_1_1_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[8];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_13_4_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[104];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_13_4_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[208];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_13_4_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 4;
  return 0;
  #else
  static uint8_t buf[416];
  *page_cnt = 4;
  return buf;
  #endif
}
uint8_t *u8g2_m_20_2_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[160];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_20_2_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[320];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_20_2_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[320];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_32_7_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[256];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_32_7_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[512];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_32_7_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 7;
  return 0;
  #else
  static uint8_t buf[1792];
  *page_cnt = 7;
  return buf;
  #endif
}
uint8_t *u8g2_m_32_6_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[256];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_32_6_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[512];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_32_6_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 6;
  return 0;
  #else
  static uint8_t buf[1536];
  *page_cnt = 6;
  return buf;
  #endif
}
uint8_t *u8g2_m_48_30_1(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 1;
  return 0;
  #else
  static uint8_t buf[384];
  *page_cnt = 1;
  return buf;
  #endif
}
uint8_t *u8g2_m_48_30_2(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 2;
  return 0;
  #else
  static uint8_t buf[768];
  *page_cnt = 2;
  return buf;
  #endif
}
uint8_t *u8g2_m_48_30_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 30;
  return 0;
  #else
  static uint8_t buf[11520];
  *page_cnt = 30;
  return buf;
  #endif
}
/* end of generated code */
