#ifndef AARCH64_OS_UTSNAME_H
#define AARCH64_OS_UTSNAME_H

struct utsname {
  char sysname[64];
  char nodename[64];
  char release[64];
  char version[64];
  char machine[64];
};

#endif
