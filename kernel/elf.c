#include "kernel.h"
#include "elf.h"
#include "printk.h"
#include "ext2.h"

bool is_elf(struct ehdr *e) {
  if(e->e_ident[0] == 0x7f && e->e_ident[1] == 'E' &&
      e->e_ident[2] == 'L' && e->e_ident[3] == 'F')
    return true;
  else
    return false;
}

void dump_phdr(struct phdr *p) {
  printk("dump phdr\n");
  printk("p_type %p\n", p->p_type);
  printk("p_offset %p\n", p->p_offset);
  printk("p_vaddr %p\n", p->p_vaddr);
  printk("p_filesz %d\n", p->p_filesz);
  printk("p_memsz %d\n", p->p_memsz);
}

void dump_ehdr(struct ehdr *e) {
  printk("dump ehdr\n");
  printk("e_entry: %d\n", e->e_entry);
  printk("e_phnum: %d\n", e->e_phnum);
  printk("e_phoff: %d\n", e->e_phoff);
}

void dump_elf(struct inode *ino) {
  printk("dump elf\n");
  if(ino == NULL)
    return;

  struct ehdr eh;
  struct phdr ph;

  if(read_inode(ino, (char *)&eh, 0, sizeof(eh)) != sizeof(eh))
    return;
  if(!is_elf(&eh))
    return;
  if(eh.e_type != ET_EXEC)
    return;

  dump_ehdr(&eh);

  int i = 0;
  u64 off = eh.e_phoff;
  for(; i < eh.e_phnum; i++, off += sizeof(ph)) {
    if(read_inode(ino, (char *)&ph, off, sizeof(ph)) != sizeof(ph))
      return;
    dump_phdr(&ph);
  }
  printk("dump elf done\n");
}
