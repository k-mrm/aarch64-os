#include "uni.h"
#include "framebuffer.h"
#include "mailbox.h"

struct framebuffer display_fb = {1920, 1080, 1920, 1080, 0, 16, 0, 0, 0, 0};
volatile u32 __attribute__((aligned(16))) mbox_msg[26];

void fb_init() {
  mbox_msg[0] = 104;
  mbox_msg[1] = 0;

  mbox_msg[2] = 0x48003;
  mbox_msg[3] = 8;
  mbox_msg[4] = 0;
  mbox_msg[5] = display_fb.phy_w;
  mbox_msg[6] = display_fb.phy_h;

  mbox_msg[7] = 0x48004;
  mbox_msg[8] = 8;
  mbox_msg[9] = 0;
  mbox_msg[10] = display_fb.w;
  mbox_msg[11] = display_fb.h;

  mbox_msg[12] = 0x48005;
  mbox_msg[13] = 4;
  mbox_msg[14] = 0;
  mbox_msg[15] = display_fb.depth;

  mbox_msg[16] = 0x40008;
  mbox_msg[17] = 4;
  mbox_msg[18] = 0;
  mbox_msg[19] = 0;

  mbox_msg[20] = 0x40001;
  mbox_msg[21] = 8;
  mbox_msg[22] = 0;
  mbox_msg[23] = 16;
  mbox_msg[24] = 0;

  mbox_msg[25] = 0;
  /*
  u32 __attribute__((aligned(16))) mbox_msg[] = {
    112,
    0, // request
    0x48003, 8, 0, display_fb.phy_w, display_fb.phy_h,
    0x48004, 8, 0, display_fb.w, display_fb.h,
    0x48005, 4, 0, display_fb.depth,
    0x40008, 4, 0, 0,
    0x40001, 8, 0, 16, 0,
    0,  // end tag
  };
  */

  mbox_write(8, (u32)(u64)mbox_msg);
  mbox_read(8);

  display_fb.phy_w = mbox_msg[5];
  display_fb.phy_h = mbox_msg[6];
  display_fb.w = mbox_msg[10];
  display_fb.h = mbox_msg[11];
  display_fb.depth = mbox_msg[15];
  display_fb.pitch = mbox_msg[19];
  display_fb.buf = (u16 *)(u64)(mbox_msg[23] & 0x3fffffff);
  display_fb.nbuf = mbox_msg[24];
}

void drawpxl(u32 x, u32 y, u16 c) {
  u32 off = y * display_fb.pitch + x * (display_fb.depth / 8);
  *(u16 *)(display_fb.buf + off) = c;
}

void fb_wtest() {
  drawpxl(120, 50, 0xffff);
  drawpxl(121, 50, 0xffff);
  drawpxl(122, 50, 0xffff);
  drawpxl(123, 50, 0xffff);
  drawpxl(124, 50, 0xffff);
  drawpxl(125, 50, 0xffff);
  drawpxl(126, 50, 0xffff);
  drawpxl(127, 50, 0xffff);
  drawpxl(128, 50, 0xffff);
  drawpxl(129, 50, 0xffff);
  drawpxl(130, 50, 0xffff);
  drawpxl(131, 50, 0xffff);
  drawpxl(131, 51, 0xffff);
  drawpxl(131, 52, 0xffff);
  drawpxl(131, 53, 0xffff);
  drawpxl(131, 54, 0xffff);
  drawpxl(131, 55, 0xffff);
  drawpxl(131, 56, 0xffff);
  drawpxl(131, 57, 0xffff);
  drawpxl(131, 58, 0xffff);
}
