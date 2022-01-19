#include "ulib.h"
#include "usys.h"

struct person {
  char *name;
  int age;
  enum { MAN, WOMAN, OTHER } sex;
};

struct person *new_person(char *name, int age, int sex) {
  struct person *p = malloc(sizeof(*p));
  if(!p)
    return NULL;

  p->name = name;
  p->age = age;
  p->sex = sex;

  return p;
}

void dump_person(struct person *p) {
  if(!p)
    return;

  printf("%s, %d, %s\n", p->name, p->age,
      (p->sex == MAN) ? "man"
      : (p->sex == WOMAN) ? "woman"
      : "other");
}

int main(void) {
  struct person *g = new_person("garin", 3141, OTHER);
  dump_person(g);
  free(g);

  exit(0);
}
