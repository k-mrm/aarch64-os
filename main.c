#include "mono.h"
#include "arm.h"
#include "gpio.h"
#include "framebuffer.h"
#include "console.h"
#include "printf.h"
#include "systimer.h"
#include "interrupt.h"

int main(void) {
  intr_init();
  fb_init();
  cs_init();

  printf("Hello, World!\n");
  printf("%d%%\n", 24000);
  printf("%d %u\n", -1, -1);
  printf("systime %d\n", systime());
  sleep(5);
  printf("systime %d\n", systime());
  printf("current EL: %d\n", cur_el());
  printf("%s%c\n", "Hello, raspi4", '!');
  printf("addr: %p\n", printf);
  printf("var_el1: %p\n", vbar_el1());

  int r = *((volatile int *)0xffffffffff000000);
  r++;

  for(;;) {}

  return 0;
}
