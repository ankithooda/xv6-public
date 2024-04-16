#include "types.h"
#include "user.h"
#include "stat.h"

int
main(int argc, char *argv[])
{
  int pid = getpid();
  dumppagetable(1);
  dumppagetable(2);
  dumppagetable(pid);
  int rc = fork();
  if (rc == 0) {
    printf(1, "Parent Process\n");
    wait();
    dumppagetable(pid);
    exit();
  } else if (rc < 0) {
    printf(1, "Forking Error \n");
    exit();
  } else {
    printf(1, "Child Process\n");
    dumppagetable(rc);
    int *a = (int*)0x1800;
    *a = 50;
    printf(1, "Value - %d\n", *a);
    exit();
  }
}
