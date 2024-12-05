//
// tests for copy-on-write fork() assignment.
//

#include "types.h"
#include "memlayout.h"
#include "user.h"

// allocate more than half of physical memory,
// then fork. this will fail in the default
// kernel, which does not support copy-on-write.
void
simpletest()
{
  uint64 phys_size = PHYSTOP - EXTMEM;
  int sz = (phys_size / 3) * 2;

  printf(1, "Allocating %d mb bytes\n", sz / (1024 * 1024));

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

// three processes all write COW memory.
// this causes more than half of physical memory
// to be allocated, so it also checks whether
// copied pages are freed.
void
threetest()
{
  uint64 phys_size = PHYSTOP - EXTMEM;
  int sz = phys_size / 4;
  int pid1, pid2;

  printf(1, "three: ");

  char *p = sbrk(sz);
  if(p == (char*)0xffffffffffffffffL){
    printf(1, "sbrk(%d) failed\n", sz);
    exit();
  }

  pid1 = fork();
  if(pid1 < 0){
    printf(1, "fork failed\n");
    exit();
  }

  // pid1 is the child process
  if(pid1 == 0){

    pid2 = fork();
    if(pid2 < 0){
      printf(1, "fork failed");
      exit();
    }

    // pid2 is the grandchild process
    if(pid2 == 0){
      for(char *q = p; q < p + (sz/5)*4; q += 4096){
        *(int*)q = getpid();
      }
      for(char *q = p; q < p + (sz/5)*4; q += 4096){
        if(*(int*)q != getpid()){
          printf(1, "wrong content\n");
          exit();
        }
      }
      exit();
    }

    // pid1 writes to it's memory
    for(char *q = p; q < p + (sz/2); q += 4096){
      *(int*)q = 9999;
    }

    // pid1 waits for pid2 to exit
    // wait();
    exit();
  }

  // Grandparent process
  for(char *q = p; q < p + sz; q += 4096){
    *(int*)q = getpid();
  }

  wait();

  sleep(1);

  for(char *q = p; q < p + sz; q += 4096){
    if(*(int*)q != getpid()){
      printf(1, "wrong content\n");
      exit();
    }
  }

  if(sbrk(-sz) == (char*)0xffffffffffffffffL){
    printf(1, "sbrk(-%d) failed\n", sz);
    exit();
  }

  // Wait for pid1 to exit if it has not already.
  // wait();
  printf(1, "ok\n");
}

char junk1[4096];
int fds[2];
char junk2[4096];
char buf[4096];
char junk3[4096];

// test whether copyout() simulates COW faults.
void
filetest()
{
  printf(1, "file: ");

  buf[0] = 99;

  for(int i = 0; i < 4; i++){
    if(pipe(fds) != 0){
      printf(1, "pipe() failed\n");
      exit();
    }
    int pid = fork();
    if(pid < 0){
      printf(1, "fork failed\n");
      exit();
    }
    if(pid == 0){
      sleep(1);
      if(read(fds[0], buf, sizeof(i)) != sizeof(i)){
        printf(1, "error: read failed\n");
        exit();
      }
      sleep(1);
      int j = *(int*)buf;
      if(j != i){
        printf(1, "error: read the wrong value\n");
        exit();
      }
      exit();
    }
    if(write(fds[1], &i, sizeof(i)) != sizeof(i)){
      printf(1, "error: write failed\n");
      exit();
    }
  }

  /* int xstatus = 0; */
  for(int i = 0; i < 4; i++) {
    wait();
    /* if(xstatus != 0) { */
    /*   exit(); */
    /* } */
  }

  if(buf[0] != 99){
    printf(1, "error: child overwrote parent\n");
    exit();
  }

  printf(1, "ok\n");
}

int
main(int argc, char *argv[])
{
  simpletest();

  // check that the first simpletest() freed the physical memory.
  simpletest();

  threetest();
  threetest();
  threetest();

  filetest();

  printf(1, "ALL COW TESTS PASSED\n");

  exit();
}
