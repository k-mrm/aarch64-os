#include "uni.h"
#include "arm.h"
#include "gpio.h"
#include "framebuffer.h"
#include "console.h"

void main(void) {
  fb_init();
  cs_init();

  csputs(&cons1, "Hello, World!\n");
  csputs(&cons1, "Hello");

  for(;;) {}
}
