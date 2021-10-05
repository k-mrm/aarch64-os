#ifndef AARCH64_OS_MM_H
#define AARCH64_OS_MM_H

#define TCR_T0SZ(n)   ((n) & 0x3f)
#define TCR_IRGN0(n)  (((n) & 0x3) << 8)
#define TCR_ORGN0(n)  (((n) & 0x3) << 10)
#define TCR_SH0(n)    (((n) & 0x3) << 12)
#define TCR_TG0(n)    (((n) & 0x3) << 14)
#define TCR_T1SZ(n)   (((n) & 0x3f) << 16)
#define TCR_A1(n)     (((n) & 0x1) << 22)
#define TCR_EPD1(n)   (((n) & 0x1) << 23)
#define TCR_IRGN1(n)  (((n) & 0x3) << 24)
#define TCR_ORGN1(n)  (((n) & 0x3) << 26)
#define TCR_SH1(n)    (((n) & 0x3) << 28)
#define TCR_TG1(n)    (((n) & 0x3) << 30)
#define TCR_IPS(n)    (((n) & 0x7) << 32)
#define TCR_AS(n)     (((n) & 0x1) << 36)
#define TCR_TBI0(n)   (((n) & 0x1) << 37)
#define TCR_TBI1(n)   (((n) & 0x1) << 38)

/*
 *  39bit(=512GB) Virtual Address
 *
 *     63   39 38    30 29    21 20    12 11       0
 *    +-------+--------+--------+--------+----------+
 *    | TTBRn | level1 | level2 | level3 | page off |
 *    +-------+--------+--------+--------+----------+
 *
 *
 */

#define PIDX(level, va) (((va) >> (39 - (level) * 9)) & 0x1ff)
#define OFFSET(va)  ((va) & 0xfff)

#define PTE_PA(pte) ((u64)(pte) & 0xfffffffff000)

/* lower attribute */
#define PTE_VALID 1   /* level 0,1,2 descriptor */
#define PTE_TABLE 2   /* level 0,1,2 descriptor */
#define PTE_V 3       /* level 3 descriptor */
#define PTE_INDX(idx) (((idx) & 7) << 2)
#define PTE_NORMAL  PTE_INDX(AI_NORMAL_NC_IDX)
#define PTE_DEVICE  PTE_INDX(AI_DEVICE_nGnRnE_IDX)
#define PTE_NS  (1 << 5)
#define PTE_AP(ap)  (((ap) & 3) << 6)
#define PTE_U   PTE_AP(1)
#define PTE_SH(sh)  (((sh) & 3) << 8)
#define PTE_AF  (1 << 10)
/* upper attribute */
#define PTE_PXN (1UL << 53)
#define PTE_UXN (1UL << 54)

#define PAGESIZE  4096    /* 4KB */
#define PAGEROUNDUP(p)  ((p + PAGESIZE - 1) & ~(PAGESIZE - 1))

#define SECTIONSIZE (2 * 1024 * 1024) /* 2MB */

/* attr index */
#define AI_DEVICE_nGnRnE_IDX  0x0
#define AI_NORMAL_NC_IDX      0x1

#define AI_DEVICE_nGnRnE  0x0
#define AI_NORMAL_NC      0x44

#ifndef __ASSEMBLER__

#include "ext2.h"

int init_userspace(u64 *pgt, char *code, u64 size);
int alloc_userspace(u64 *pgt, u64 va, struct inode *ino, u64 srcoff, u64 size);
void load_userspace(u64 *pgt);
void free_userspace(u64 *pgt, u64 size);
void forget_userspace(void);
char *map_ustack(u64 *pgt);
char *cp_userspace(u64 *newpgt, u64 *oldpgt);
char *cp_ustack(u64 *newpgt, u64 *oldpgt);
void dump_ustack(u64 *pgt);

#endif

#endif
