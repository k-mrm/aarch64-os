#include "arm.h"
#include "uni.h"
#include "interrupt.h"
#include "printf.h"

extern u64 vectortable;

void intr_init() {
  set_vbar_el1(vectortable);
}

void sync_handler() {
  printf("sync\n");
}
