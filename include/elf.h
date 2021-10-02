#ifndef AARCH64_OS_ELF_H
#define AARCH64_OS_ELF_H

#include "kernel.h"

/* e_type */
#define ET_NONE 0
#define ET_REL  1
#define ET_EXEC 2
#define ET_DYN  3
#define ET_CORE 4

/* p_type */
#define PT_NULL 0
#define PT_LOAD 1
#define PT_DYNAMIC  2
#define PT_INTERP 3
#define PT_NOTE 4
#define PT_SHLIB  5
#define PT_PHDR 6

/* p_flags */
#define PF_X  0x1
#define PF_W  0x2
#define PF_R  0x4
#define PF_MASKOS 0x00ff0000
#define PF_MASKPROC 0xff000000

struct ehdr {
  unsigned char e_ident[16];
  u16 e_type;
  u16 e_machine;
  u32 e_version;
  u64 e_entry;
  u64 e_phoff;
  u64 e_shoff;
  u32 e_flags;
  u16 e_ehsize;
  u16 e_phentsize;
  u16 e_phnum;
  u16 e_shentsize;
  u16 e_shnum;
  u16 e_shstrndx;
} __attribute__((packed));

struct phdr {
  u32 p_type;
  u32 p_flags;
  u64 p_offset;
  u64 p_vaddr;
  u64 p_paddr;
  u64 p_filesz;
  u64 p_memsz;
  u64 p_align;
} __attribute__((packed));

bool is_elf(struct ehdr *eh);

#endif
