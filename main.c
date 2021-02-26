#include "uni.h"
#include "arm.h"
#include "gpio.h"
#include "framebuffer.h"
#include "console.h"
#include "systimer.h"

int main(void) {
  fb_init();
  cs_init();

  csputs(&cons1, "Hello, World!\n");
  csputs(&cons1, "Hello");
  sleep(10);
  csputc(&cons1, '0' + cur_el());

  for(;;) {}

  return 0;
}
