#include "types.h"
#include "user.h"
#include "stat.h"
#include "memlayout.h"

// allocate more than half of physical memory,
// then fork. this will fail in the default
// kernel, which does not support copy-on-write.
void
simpletest()
{
  uint64 phys_size = PHYSTOP - EXTMEM;
  int sz = (phys_size / 3) * 2;

  printf(1, "simple: ");

  char *p = sbrk(sz);
  if(p == (char*)0xffffffffffffffffL){
    printf(1, "sbrk(%d) failed\n", sz);
    exit();
  }

  for(char *q = p; q < p + sz; q += 4096){
    *(int*)q = getpid();
  }

  int pid = fork();
  if(pid < 0){
    printf(1, "fork() failed\n");
    exit();
  }

  if(pid == 0)
    exit();

  wait();

  if(sbrk(-sz) == (char*)0xffffffffffffffffL){
    printf(1, "sbrk(-%d) failed\n", sz);
    exit();
  }

  printf(1, "ok\n");
}

int
main(int argc, char *argv[])
{
  simpletest();
  simpletest();

  exit();

}
