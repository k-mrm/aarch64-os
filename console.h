#ifndef UNI_CONSOLE_H
#define UNI_CONSOLE_H

#include "uni.h"
#include "framebuffer.h"
#include "font.h"

struct console {
  struct framebuffer *fb;
  struct font *font;
  u32 cur_x;
  u32 cur_y;
};

extern struct console cons1;

void cs_init(void);
void csputc(struct console *cs, char c);
void csputs(struct console *cs, char *s);

#endif
