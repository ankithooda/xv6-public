#include "types.h"
#include "user.h"
#include "stat.h"

int main(int argc, char *argv[])
{
  settickets(30);
  printf(1, " >>>>>>>>>>>>>>>>>>>>> PROCESS 1\n");
  int cpid = fork();

  if (cpid == 0) {
    settickets(20);
    printf(1, " >>>>>>>>>>>>>>>>>>>>> PROCESS 2\n");

    int gpid = fork();

    if (gpid == 0) {
      settickets(10);
      printf(1, " >>>>>>>>>>>>>>>>>>>>> PROCESS 3\n");
    } else if (gpid < 0) {
      printf(1, "Error in spawning grandchild\n");
    }
  } else if (cpid < 0) {
    printf(1, "Error in spawning child\n");
  }

  for (;;) {;}

  exit();
  return 0;
}
