#include "kernel.h"
#include "file.h"
#include "ext2.h"
#include "proc.h"
#include "stat.h"
#include "string.h"
#include "fcntl.h"
#include "dirent.h"

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
  if(!f || !f->readable)
    return -1;

  struct inode *ino = f->ino;
  if(!ino)
    return -1;
  
  int n = read_inode(ino, buf, f->off, sz);
  if(n < 0)
    return -1;   

  f->off += n;

  return n;
}

int write_file(struct file *f, char *buf, u64 sz) {
  if(!f || !f->writable)
    return -1;

  struct inode *ino = f->ino;
  if(!ino)
    return -1;

  /* TODO */

  return 0;
}

int read(int fd, char *buf, u64 sz) {
  if(fd < 3)
    return console_read(buf, sz);
  struct proc *p = curproc;
  struct file *f = p->ofile[fd];
  return read_file(f, buf, sz);
}

int write(int fd, char *buf, u64 sz) {
  if(fd < 3)
    return console_write(buf, sz);
  struct proc *p = curproc;
  struct file *f = p->ofile[fd];
  return write_file(f, buf, sz);
}

int getdents(int fd, struct dirent *dir, u64 count) {
  ;
}

int fstat(int fd, struct stat *st) {
  struct proc *p = curproc;
  struct file *f = p->ofile[fd];
  struct inode *ino = f->ino;

  st->st_dev = 0; /* TODO */
  st->st_ino = 0; /* TODO */
  st->st_mode = ino->i_mode;
  st->st_size = ino->i_size;
  st->st_nlink = ino->i_links_count;
}

int open(char *path, int flags) {
  struct inode *ino = path2inode(path);
  if(!ino)
    return -1;

  struct file *f = alloc_file();
  struct proc *p = curproc;

  f->ino = ino;
  f->off = 0;

  switch(flags & O_ACCMODE) {
    case O_RDONLY:
      f->readable = 1;
      break;
    case O_WRONLY:
      f->writable = 1;
      break;
    case O_RDWR:
      f->readable = 1;
      f->writable = 1;
      break; 
  }

  if((flags & O_DIRECTORY) && !(ino->i_mode & S_IFDIR))
    return -1;

  int fd;
  for(fd = 3; fd < 16; fd++) {
    if(p->ofile[fd] == NULL)
      goto found;
  }

  return -1;

found:
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
