#include "types.h"
#include "user.h"
#include "stat.h"
#include "pstat.h"

int
main(int argc, char *argv[])
{
  printf(1, "Hello World %d\n", getpinfo(struct pstat *));
  exit();
}
