#include "types.h"
#include "user.h"
#include "stat.h"

int
main(int argc, char *argv[])
{
  int pid = getpid();
  char *p = sbrk(40000);
  printf(1, "Alloced more mem %p\n", p);

  char *a;

  for (int i = 20000; i <= 30000; i++) {
    a = (char*)i;
    *a = 'X';
  }

  dumppagetable(pid);

  int rc = fork();

  if (rc == 0) {
    printf(1, "Child Print all tables\n");

    dumppagetable(getpid());
    dumppagetable(3);

    char *a;
    for (int i = 28000; i <= 33000; i++) {
      a = (char*)i;
      *a = 'Z';
    }
    int gc = fork();
    if (gc == 0) {
      printf(1, "Printing grandchild \n");
      dumppagetable(getpid());
      exit();
    } else if (gc < 0) {
      printf(1, "Error in forking grandchild\n");
    } else {
      wait();
      dumppagetable(getpid());
      exit();
    }

  } else if (rc < 0) {
    printf(1, "Forking Error \n");
    exit();

  } else {
    wait();
    dumppagetable(pid);
    exit();
  }
}
