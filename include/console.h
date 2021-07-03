#ifndef MONO_CONSOLE_H
#define MONO_CONSOLE_H

#include "mono.h"
#include "font.h"

#define USE_UART

struct console {
#ifdef USE_UART
  int uartid;
#else
  struct framebuffer *fb;
  struct font *font;

  u32 cur_x;
  u32 cur_y;

  u32 w;
  u32 h;

  u32 initx;

  u32 lineh;

  u32 bpl;
#endif
};

extern struct console cons1;

void console_init(void);
void csputc(struct console *cs, char c);
void csputs(struct console *cs, char *s);

#endif
