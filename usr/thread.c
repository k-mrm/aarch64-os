#include "usys.h"
#include "ulib.h"

static int counter;

int th_create(int *tid, void *fn) {
  void *stack = malloc(0x1000);
  int t = clone(fn, stack);
  if(t < 0)
    return -1;
  *tid = t;
  return 0;
}

void fn0() {
  printf("fn0\n");
  for(int i = 0; i < 10000000; i++)
    counter++;
  exit(0);
}

void fn1() {
  printf("fn1\n");
  for(int i = 0; i < 10000000; i++)
    counter++;
  exit(0);
}

void fn2() {
  printf("fn2\n");
  for(int i = 0; i < 10000000; i++)
    counter++;
  exit(0);
}

void fn3() {
  printf("fn3\n");
  for(int i = 0; i < 10000000; i++)
    counter++;
  exit(0);
}

int main(void) {
  int tid0, tid1, tid2, tid3;

  int a = ticks();

  if(th_create(&tid0, fn0) < 0)
    exit(1);
  if(th_create(&tid1, fn1) < 0)
    exit(1);
  if(th_create(&tid2, fn2) < 0)
    exit(1);
  if(th_create(&tid3, fn3) < 0)
    exit(1);

  waitpid(tid0, NULL);
  waitpid(tid1, NULL);
  waitpid(tid2, NULL);
  waitpid(tid3, NULL);

  int b = ticks();

  printf("counter: %d %d\n", counter, b-a);

  exit(0);
}
