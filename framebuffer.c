#include "uni.h"
#include "framebuffer.h"
#include "mailbox.h"
#include "font.h"

struct framebuffer display_fb = {1920, 1080, 1440, 810, 0, 16, 0, 0, 0, 0};
volatile u32 __attribute__((aligned(16))) mbox_msg[26];

void fb_init() {
  mbox_msg[0] = 104;  /* size of mbox_msg */
  mbox_msg[1] = 0;

  mbox_msg[2] = PROP_FB_SET_PHY_WH;
  mbox_msg[3] = 8;
  mbox_msg[4] = 0;
  mbox_msg[5] = display_fb.phy_w;
  mbox_msg[6] = display_fb.phy_h;

  mbox_msg[7] = PROP_FB_SET_VIRT_WH;
  mbox_msg[8] = 8;
  mbox_msg[9] = 0;
  mbox_msg[10] = display_fb.w;
  mbox_msg[11] = display_fb.h;

  mbox_msg[12] = PROP_FB_SET_DEPTH;
  mbox_msg[13] = 4;
  mbox_msg[14] = 0;
  mbox_msg[15] = display_fb.depth;

  mbox_msg[16] = PROP_FB_GET_PITCH;
  mbox_msg[17] = 4;
  mbox_msg[18] = 0;
  mbox_msg[19] = 0;

  mbox_msg[20] = PROP_FB_ALLOC_BUFFER;
  mbox_msg[21] = 8;
  mbox_msg[22] = 0;
  mbox_msg[23] = 16;
  mbox_msg[24] = 0;

  mbox_msg[25] = PROP_END;
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

void drawpxl(struct framebuffer *fb, u32 x, u32 y, u16 c) {
  u32 off = y * fb->pitch + x * (fb->depth >> 3);
  *(u16 *)((u8 *)fb->buf + off) = c;
}

void drawchar(struct framebuffer *fb, struct font *font, u32 *x, u32 *y, char c) {
  if(c == '\n') {
    *x = 20;
    *y += 10;
    return;
  }

  const char *glyph = font->data[c];

  for(int i = 0; i < font->h; i++) {
    for(int j = 0; j < font->w; j++) {
      if(glyph[i] & (1 << j))
        drawpxl(fb, *x + j, *y + i, 0xffff);
    }
  }

  *x += 8;
}

void drawstr(struct framebuffer *fb, struct font *font, u32 *x, u32 *y, char *s) {
  while(*s) {
    drawchar(fb, font, x, y, *s++);
  }
}
