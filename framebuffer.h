#ifndef UNI_FRAMEBUFFER_H
#define UNI_FRAMEBUFFER_H

#include "uni.h"

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
void fb_wtest(void);

#endif
