#ifndef AARCH64_OS_FRAMEBUFFER_H
#define AARCH64_OS_FRAMEBUFFER_H

#include "mono.h"
#include "font.h"

struct framebuffer {
  u32 phy_w;
  u32 phy_h;
  u32 w;
  u32 h;
  u32 pitch;
  u32 depth;
  u32 off_x;
  u32 off_y;
  u16 *buf;
  u32 nbuf;
};

extern struct framebuffer display_fb;

void fb_init(void);
void drawpxl(struct framebuffer *fb, u32 x, u32 y, u16 c);
void drawchar(struct framebuffer *fb, struct font *font, u32 x, u32 y, char c);

#endif
