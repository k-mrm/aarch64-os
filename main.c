#include "uni.h"
#include "gpio.h"
#include "framebuffer.h"

void main(void) {
  fb_init();

  fb_wtest();

  for(;;) {}
}
