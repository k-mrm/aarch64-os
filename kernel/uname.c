#include "kernel.h"
#include "utsname.h"

struct utsname aarch64_os =  {
  .sysname = "aarch64-os",
  .nodename = "",
  .release = "0.0.1",
  .version = __TIME__ "" __DATE__,
  .machine = "aarch64",
};

int uname(struct utsname *u) {
  if(!u)
    return -1;

  memcpy(u, aarch64_os, sizeof(*u));
  return 0;
}
