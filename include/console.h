#ifndef MONO_CONSOLE_H
#define MONO_CONSOLE_H

#include "mono.h"
#include "framebuffer.h"
#include "font.h"

struct console {
  struct framebuffer *fb;
  struct font *font;

  u32 cur_x;
  u32 cur_y;

  u32 w;
  u32 h;

  u32 initx;

  u32 lineh;

  u32 bpl;
};

extern struct console cons1;

void console_init(void);
void csputc(struct console *cs, char c);
void csputs(struct console *cs, char *s);

#endif
