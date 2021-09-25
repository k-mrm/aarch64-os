#include "string.h"
#include "kernel.h"
#include "font.h"
#include "console.h"
#include "driver/uart.h"

struct console cons1;

void console_init() {
#ifdef USE_UART
  uart_init();
#else
  fb_init();
  font_init();
  cons1.fb = &display_fb;
  cons1.font = &default_font;
  cons1.cur_x = 0;
  cons1.cur_y = 0;
  cons1.w = display_fb.w;
  cons1.h = display_fb.h;
  cons1.initx = 0;
  cons1.lineh = 10;
  cons1.bpl = display_fb.pitch * cons1.lineh;
#endif
}

#ifdef USE_UART

void csputc(struct console *cs, char c) {
  if(c == 127) {  /* 127 == BACKSPACE */  
    uart_putc('\b');
    uart_putc(' ');
    uart_putc('\b');
  } else {
    uart_putc(c);
  }
}

void csputs(struct console *cs, char *s) {
  uart_puts(s);
}

char csgetc(struct console *cs) {
  return uart_getc();
}

int cswrite(struct console *cs, char *s, u64 size) {
  u64 i;
  for(i = 0; i < size; i++) {
    uart_putc(*s++);
  }

  return i;
}

int csread(struct console *cs, char *buf, u64 size) {
  u64 i;
  for(i = 0; i < size; i++, buf++) {
    char c = csgetc(cs);
    if(c == '\r')
      c = '\n';

    *buf = c;
    csputc(cs, c);

    if(c == '\n')
      break;
  }

  return i;
}

#else

static void csscroll(struct console *cs) {
  memmove(cs->fb->buf, (char *)cs->fb->buf + cs->bpl, cs->fb->pitch * (cs->h - cs->lineh));
  memset((char *)cs->fb->buf + cs->fb->pitch * (cs->h - cs->lineh), 0, cs->bpl);

  cs->cur_x = cs->initx;
  cs->cur_y = cs->h - cs->lineh;
}

static void csnewline(struct console *cs) {
  if(cs->cur_y == cs->h) {
    csscroll(cs);
  } else {
    cs->cur_x = cs->initx;
    cs->cur_y += cs->lineh;
  }
}

void csputc(struct console *cs, char c) {
  if(c == '\n') {
    csnewline(cs);
    return;
  }

  drawchar(cs->fb, cs->font, cs->cur_x, cs->cur_y, c);
  cs->cur_x += cs->font->w;
}

void csputs(struct console *cs, char *s) {
  while(*s) {
    csputc(cs, *s++);
  }
}

#endif

int _write(char *s, u64 size) {
  return cswrite(&cons1, s, size);
}

int _read(char *buf, u64 size) {
  return csread(&cons1, buf, size);
}
