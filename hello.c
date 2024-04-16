#include "types.h"
#include "user.h"
#include "stat.h"

int
main(int argc, char *argv[])
{
  int pid = getpid();
  char *p = sbrk(0x500000);
  printf(1, "Alloced more mem %p\n", p);

  char *a;

  for (int i = 0x402040; i <= 0x40A000; i++) {
    a = (char*)i;
    *a = 'X';
  }

  dumppagetable(pid);

  int rc = fork();

  if (rc == 0) {
    printf(1, "Child Print all tables\n");
    dumppagetable(getpid());
    dumppagetable(3);
    exit();

  } else if (rc < 0) {
    printf(1, "Forking Error \n");
    exit();

  } else {
    wait();
    dumppagetable(pid);
    exit();
  }
}
