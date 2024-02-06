#include "types.h"
#include "user.h"
#include "stat.h"
#include "pstat.h"
#include "param.h"

int
main(int argc, char *argv[])
{
  struct pstat p;
  printf(1, "Hello World %d-%d\n", getpinfo(&p), &p);
  for (int i = 0; i < NPROC; i++) {
    if (p.inuse[i] == 1) {
      printf(1, "%d\t%d\t%d\t%d\n", p.inuse[i], p.pid[i], p.tickets[i], p.ticks[i]);
    }
  }
  exit();
}
