#include "mono.h"
#include "arm.h"
#include "gpio.h"
#include "framebuffer.h"
#include "console.h"
#include "printf.h"
#include "systimer.h"
#include "interrupt.h"

int test;

int main(void) {
  fb_init();
  cs_init();
  intr_init();

  printf("Hello, World!\n");
  printf("%d%%\n", 24000);
  printf("%d %u\n", -1, -1);
  sleep(5);
  printf("current EL: %d\n", cur_el());
  printf("%s%c\n", "Hello, raspi4", '!');
  printf("addr: %p\n", printf);

  for(;;) {}

  return 0;
}
