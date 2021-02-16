#ifndef UNI_FONT_H
#define UNI_FONT_H

#include "uni.h"

struct font {
  char (*data)[8];
  u8 w;
  u8 h;
};

extern struct font default_font;

void font_init(void);

#endif
