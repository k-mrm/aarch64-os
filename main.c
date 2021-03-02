#include "uni.h"
#include "arm.h"
#include "gpio.h"
#include "framebuffer.h"
#include "console.h"
#include "systimer.h"

int test;

int main(void) {
  fb_init();
  cs_init();

  csputs(&cons1, "Hello, World!\n");
  csputs(&cons1, "Hello");
  sleep(5);
  csputc(&cons1, '0' + cur_el());
  csputc(&cons1, '0' + (test == 0));

  for(;;) {}

  return 0;
}
