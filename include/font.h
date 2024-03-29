#ifndef AARCH64_OS_FONT_H
#define AARCH64_OS_FONT_H

#include "kernel.h"

struct font {
  char (*data)[8];
  u8 w;
  u8 h;
};

extern struct font default_font;

void font_init(void);

#endif
