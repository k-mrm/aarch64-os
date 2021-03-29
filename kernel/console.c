#include "mono.h"
#include "framebuffer.h"
#include "font.h"
#include "console.h"

struct console cons1;

void console_init() {
  fb_init();
  font_init();
  cons1.fb = &display_fb;
  cons1.font = &default_font;
  cons1.cur_x = 20;
  cons1.cur_y = 20;
}

void csputc(struct console *cs, char c) {
  drawchar(cs->fb, cs->font, &cs->cur_x, &cs->cur_y, c);
}

void csputs(struct console *cs, char *s) {
  drawstr(cs->fb, cs->font, &cs->cur_x, &cs->cur_y, s);
}
