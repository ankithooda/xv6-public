#include "types.h"
#include "user.h"
#include "stat.h"
#include "pstat.h"
#include "param.h"

int
main(int argc, char *argv[])
{
  struct pstat p;
  struct pstat *padd = (struct pstat *)0x80000001;
  printf(1, "Address in userspace %p\n", padd);

  getpinfo(&p);

  printf(1, "PID\tTICKETS\tTICKS\n");
  for (int i = 0; i < NPROC; i++) {
    if (p.inuse[i] == 1) {
      printf(1, "%d\t%d\t%d\n", p.pid[i], p.tickets[i], p.ticks[i]);
    }
  }
  exit();
}
