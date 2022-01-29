#include "usys.h"
#include "ulib.h"

void worker_thread() {
  volatile u64 i;
  for(i = 0; i < 100000000; i++)
    ;
  printf("thread done! %d\n", i);
  exit(0);
}

int th_create(int *tid, void *fn) {
  void *stack = malloc(0x1000);
  int t = clone(fn, stack);
  if(t < 0)
    return -1;
  *tid = t;
  return 0;
}

int main() {
  int tid[16];
  for(int i = 0; i < 16; i++)
    th_create(&tid[i], worker_thread);

  for(int i = 0; i < 16; i++)
    waitpid(tid[i], NULL);

  exit(0);
}
