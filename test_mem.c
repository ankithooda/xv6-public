#include "types.h"
#include "user.h"
#include "stat.h"
#include "pstat.h"
#include "param.h"

int
main(int argc, char *argv[])
{
  uint *addr;
  addr = malloc(50);
  *(addr + 0) = 1234;
  *(addr + 49) = 9870;
  printf(1, "%d\n", *(addr + 0));
  printf(1, "%d\n", *(addr + 49));
  exit();
}
