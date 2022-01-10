#include "ulib.h"
#include "usys.h"
#include "utsname.h"

int main(void) {
  struct utsname u;
  uname(&u);
  puts(u.sysname);
  uname(0xfffffffffffffffff);
  exit(0);
}
