#include "kernel.h"
#include "file.h"
#include "fs.h"
#include "proc.h"
#include "stat.h"
#include "string.h"
#include "fcntl.h"
#include "dirent.h"
#include "cdev.h"
#include "printk.h"

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

int allocfd(struct proc *p) {
  for(int newfd = 0; newfd < NOFILE; newfd++) {
    if(p->ofile[newfd] == NULL)
      return newfd;
  }

  return -1;
}

int read_file(struct file *f, char *buf, u64 sz) {
  if(!f || !f->readable)
    return -1;

  struct inode *ino = f->ino;
  if(!ino)
    return -1;
  
  int n;
  switch(ino->mode & S_IFMT) {
    case S_IFREG:
    case S_IFDIR:
      n = read_inode(ino, buf, f->off, sz);
      if(n < 0)
        return -1;

      f->off += n;
      break;
    case S_IFCHR:
      n = cdevsw[ino->major].read(f, buf, sz);
      break;
    default:
      panic("unknown ftype");
  }

  return n;
}

int write_file(struct file *f, char *buf, u64 sz) {
  if(!f || !f->writable)
    return -1;

  struct inode *ino = f->ino;
  if(!ino)
    return -1;

  int n = 0;
  switch(ino->mode & S_IFMT) {
    case S_IFREG:
    case S_IFDIR:
      n = 0;
      break;
    case S_IFCHR:
      n = cdevsw[ino->major].write(f, buf, sz);
      break;
    default:
      panic("unknown ftype");
  }

  return n;
}

struct file *dup_file(struct file *f) {
  f->ref++;
  return f;
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

int fstat(int fd, struct stat *st) {
  struct proc *p = curproc;
  struct file *f = p->ofile[fd];
  struct inode *ino = f->ino;

  st->st_dev = 0; /* TODO */
  st->st_ino = ino->inum; /* TODO */
  st->st_mode = ino->mode;
  st->st_size = ino->size;
  st->st_nlink = ino->links_count;
  st->st_blksize = sb.bsize;
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

  if((flags & O_DIRECTORY) && !S_ISDIR(ino->mode))
    return -1;

  int fd;
  for(fd = 0; fd < NOFILE; fd++) {
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

int mknod(char *path, int mode, int dev) {
  struct inode *ino = fs_mknod(path, mode, dev);
  if(!ino)
    return -1;

  return 0;
}

int mkdir(char *path) {
  struct inode *ino = fs_mkdir(path);
  if(!ino)
    return -1;

  return 0;
}

int dup(int fd) {
  struct proc *p = curproc;
  int newfd = allocfd(p);
  printk("ddddup %d %d\n", fd, newfd);
  if(newfd < 0)
    return -1;

  p->ofile[newfd] = dup_file(p->ofile[fd]);

  return newfd;
}

void file_init() {
  memset(ftable, 0, sizeof(ftable));
}
