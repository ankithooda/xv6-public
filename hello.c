#include "types.h"
#include "user.h"
#include "stat.h"

int
main(int argc, char *argv[])
{
  int pid = getpid();
  dumppagetable(pid);
  sbrk(0x500000);

  char *p = (char *)0x400000;

  printf(1, "Allocating mem\n");
  printf(1, "Pointer %x\n", (int)p);

  p[0] = 'A';
  p[1] = 'H';
  p[2] = '\n';

  dumppagetable(pid);
  exit();

}
