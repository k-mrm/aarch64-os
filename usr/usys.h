#ifndef AARCH64_OS_USR_USYS_H
#define AARCH64_OS_USR_USYS_H

#include "ulib.h"

struct stat;
struct utsname;

int getpid(void);
int write(int fd, char *s, u64 size);
int read(int fd, char *buf, u64 size);
int exit(int ret);
int fork(void);
int wait(int *status);
int exec(char *path, char **argv);
int open(char *path, int flags);
int close(int fd);
int fstat(int fd, struct stat *st);
int uname(struct utsname *u);
int chdir(char *path);
int mkdir(char *path);
int mknod(char *path, int mode, int dev);
int dup(int fd);

#endif
