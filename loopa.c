#include "types.h"
#include "user.h"
#include "stat.h"

int main(int argc, char *argv[])
{
  settickets(30);
  int cpid = fork();

  if (cpid == 0) {
    settickets(20);

    int gpid = fork();

    if (gpid == 0) {
      settickets(10);
    } else if (gpid < 0) {
      printf(1, "Error in spawning grandchild\n");
    }
  } else if (cpid < 0) {
    printf(1, "Error in spawning grandchild\n");
  }

  for (;;) {;}

  exit();
  return 0;
}
