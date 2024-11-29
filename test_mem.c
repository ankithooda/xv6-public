#include "types.h"
#include "user.h"
#include "stat.h"
#include "pstat.h"
#include "param.h"

int
main(int argc, char *argv[])
{
  int a[100];
  struct pstat *p;
  p = malloc(10 * sizeof(struct pstat));
  printf(1, "init stack vars %p - %p - %p\n", &argc, argv, &p);
  printf(1, "stack data %p - %p\n", a, a+99);
  printf(1, "heap address %p\n", p);

  dumppagetable(getpid());
  for (int i = 0; i < 10; i++) {
    getpinfo(p+i);
    printf(1, "malloced address - %p\n", p+i);
  }
  printf(1, "##################\n#################\n###########################\n");
  dumppagetable(getpid());
  exit();
}
