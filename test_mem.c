#include "types.h"
#include "user.h"
#include "stat.h"
#include "pstat.h"
#include "param.h"

int
main(int argc, char *argv[])
{
  /*uint *addr;
  addr = malloc(50 * sizeof(uint));
  *(addr + 0) = 1234;
  *(addr + 49) = 9870;
  printf(1, "%d\n", *(addr + 0));
  printf(1, "%d\n", *(addr + 49));
  free(addr);
  */
  struct pstat *p;
  p = malloc(1000 * sizeof(struct pstat));

  for (int i = 0; i < 1000; i++) {
    getpinfo(p+i);
  }


  /*printf(1, "PID\tTICKETS\tTICKS\n");
  for (int i = 0; i < NPROC; i++) {
    if (p->inuse[i] == 1) {
      printf(1, "%d\t%d\t%d\n", p->pid[i], p->tickets[i], p->ticks[i]);
    }
    }*/
  free(p);

  exit();
}
