#include "usys.h"
#include "ulib.h"

char stack0[256];
char stack1[256];

int counter;

int th_create(int *tid, void *fn, void *stack) {
  int t = clone(fn, stack);
  if(t < 0)
    return -1;
  *tid = t;
  return 0;
}

void fn0() {
  for(int i = 0; i < 10000000; i++)
    counter++;
  exit(0);
}

void fn1() {
  for(int i = 0; i < 10000000; i++)
    counter++;
  exit(0);
}

int main(void) {
  int tid0, tid1;

  if(th_create(&tid0, fn0, stack0) < 0)
    exit(1);
  if(th_create(&tid1, fn1, stack1) < 0)
    exit(1);

  waitpid(tid0, NULL);
  waitpid(tid1, NULL);

  printf("counter: %d\n", counter);

  exit(0);
}
