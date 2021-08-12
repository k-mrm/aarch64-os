#include "mono.h"
#include "aarch64.h"
#include "mm.h"

#define TCR_T0SZ(n)   ((n) & 0x3f)
#define TCR_IRGN0(n)  (((n) & 0x3) << 8)
#define TCR_ORGN0(n)  (((n) & 0x3) << 10)
#define TCR_SH0(n)    (((n) & 0x3) << 12)
#define TCR_TG0(n)    (((n) & 0x1) << 14)
#define TCR_T1SZ(n)   (((n) & 0x3f) << 16)
#define TCR_A1(n)     (((n) & 0x1) << 22)
#define TCR_EPD1(n)   (((n) & 0x1) << 23)
#define TCR_IRGN1(n)  (((n) & 0x3) << 24)
#define TCR_ORGN1(n)  (((n) & 0x3) << 26)
#define TCR_SH1(n)    (((n) & 0x3) << 28)
#define TCR_TG1(n)    (((n) & 0x1) << 30)
#define TCR_IPS(n)    (((n) & 0x7) << 32)
#define TCR_AS(n)     (((n) & 0x1) << 36)
#define TCR_TBI0(n)   (((n) & 0x1) << 37)
#define TCR_TBI1(n)   (((n) & 0x1) << 38)

static __attribute__((aligned(4096))) u64 l1_pgt[512];
static __attribute__((aligned(4096))) u64 l2_pgt[1024];

extern char *ktext_end;

void set_kpgt(u64 *k) {
  set_ttbr1_el1((u64)k);
}

void set_upgt(u64 *u) {
  set_ttbr0_el1((u64)u);
}

void kpgt_init() {
  ;
  set_kpgt(0);
}

void upgt_init() {
  ;
  set_upgt(0);
}

void page_map(u64 *pgt, u64 va, u64 pa, u64 size, int perm) {
  ;
}

static void mmu_enable() {
  ;
}

void mm_init() {
  kpgt_init();
  upgt_init();

  u64 tcr = TCR_T0SZ(25) | TCR_IRGN0(1) | TCR_ORGN0(1) | TCR_SH0(3) | TCR_EPD1(1);
  set_tcr_el1(tcr);

  isb();

  mmu_enable();
}
