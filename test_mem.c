#include "types.h"
#include "user.h"
#include "stat.h"
#include "pstat.h"
#include "param.h"

int
main(int argc, char *argv[])
{
  int *addr = 0x0;
  *addr = 1234;
  //printf(1, "%d\n", *addr);
  exit();
}
