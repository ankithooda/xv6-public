#include "types.h"
#include "user.h"
#include "stat.h"

int main(int argc , char **argv) {

  int rc = fork();

  if (rc == 0) {
    // child
    int *c = (int *)0x0;
    printf(1, "%d\n", *c);

    printf(1, "Child Process\n");
    exit();
  } else if (rc < 0) {
    printf(1, "Error in fork\n");
  } else {
    wait();
    printf(1, "Parent Process\n");
    exit();
  }
}
