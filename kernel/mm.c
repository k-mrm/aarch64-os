#include "mono.h"
#include "aarch64.h"
#include "mm.h"

static __attribute__((aligned(4096))) u64 l1_pgt[512];
static __attribute__((aligned(4096))) u64 l2_pgt[1024];

extern char *ktext_end;

void kpgt_init() {
  ;
}

void upgt_init() {
  ;
}

void page_map(u64 *pgt, u64 va, u64 pa, u64 size, int perm) {
  ;
}

void mmu_enable() {
  ;
}

void set_kpgt(u64 *k) {
  set_ttbr1_el1((u64)k);
}

void set_upgt(u64 *u) {
  set_ttbr0_el1((u64)u);
}
