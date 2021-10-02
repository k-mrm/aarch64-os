#include "kernel.h"
#include "elf.h"
#include "printk.h"

bool is_elf(struct ehdr *e) {
  if(e->e_ident[0] == 0x7f && e->e_ident[1] == 'E' &&
      e->e_ident[2] == 'L' && e->e_ident[3] == 'F')
    return true;
  else
    return false;
}
