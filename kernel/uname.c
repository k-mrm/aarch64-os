#include "kernel.h"
#include "utsname.h"
#include "proc.h"

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

  if(copyout(myproc(), u, &aarch64_os, sizeof(*u)) < 0)
    return -1;
  return 0;
}
