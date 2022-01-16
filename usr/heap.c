#include "ulib.h"
#include "usys.h"

int main(void) {
  char *p = sbrk(1000);
  for(int i = 0; i < 1000; i++)
    p[i] = 'a';
  for(int i = 0; i < 1000; i++)
    printf("%c", p[i]);
  printf("\n");

  exit(0);
}
