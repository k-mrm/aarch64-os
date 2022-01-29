#include "ulib.h"
#include "usys.h"

int main(void) {
  volatile char *a = (volatile char *)0x1919191919;

  *a = 100; // fault

  printf("??? %c\n", *a);
  exit(-1);
}
