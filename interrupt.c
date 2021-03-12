#include "arm.h"
#include "mono.h"
#include "interrupt.h"
#include "printf.h"

void intr_init() {
  // set_vbar_el1(&vectortable);
}

void sync_handler() {
  printf("sync\n");
}
