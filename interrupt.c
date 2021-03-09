#include "arm.h"
#include "uni.h"
#include "interrupt.h"

extern u64 vectortable;

void intr_init() {
  set_vbar_el1(vectortable);
}
