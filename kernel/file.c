#include "kernel.h"
#include "file.h"
#include "proc.h"

struct file ftable[NFILE];

struct file *alloc_file() {
  for(int i = 0; i < NFILE; i++) {
    if(ftable[i].ref == 0) {
      ftable[i].ref = 1;
      return &ftable[i];
    }
  }

  return NULL;
}

int read_file(struct file *f, char *buf, u64 sz) {
  struct inode *ino = f->ino;
  
  int n = read_inode(ino, buf, f->off, sz);
  if(n < 0)
    return -1;   

  f->off += n;

  return n;
}

int write_file(struct file *f, char *buf, u64 sz) {
  struct inode *ino = f->ino;

  /* TODO */

  return 0;
}

int read(int fd, char *buf, u64 sz) {
  struct proc *p = curproc;
  struct file *f = p->ofile[fd];
  return read_file(f, buf, sz);
}

int write(int fd, char *buf, u64 sz) {
  struct proc *p = curproc;
  struct file *f = p->ofile[fd];
  return write_file(f, buf, sz);
}

int open(char *path) {
  struct file *f = alloc_file();
  struct proc *p = curproc;

  f->ino = path2inode(path);
  f->off = 0;

  int fd;
  for(fd = 0; fd < 16; fd++) {
    if(p->ofile[fd] == NULL)
      break;
  }
  p->ofile[fd] = f;

  return fd;
}

int close(int fd) {
  struct proc *p = curproc;
  struct file *f = p->ofile[fd];
  if(f->ref == 0)
    return -1;
  if(--f->ref > 0)
    return 0;

  p->ofile[fd] = NULL;
  memset(f, 0, sizeof(*f));
  return 0;
}

void file_init() {
  memset(ftable, 0, sizeof(ftable));
}
