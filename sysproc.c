#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;

  // For negative arguments to sbrk (reducing process size)
  // do it immediately.
  // For positive arguments to sbrk (increasing process size)
  // do it lazily and actually allocate memory in trap.c
  if (n < 0) {
    if(growproc(n) < 0)
      return -1;
  } else {
      myproc()->sz = addr + n;
  }
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_alarm(void)
{
  int ticks;
  void (*handler)();

  if(argint(0, &ticks) < 0)
    return -1;
  if(argptr(1, (char**)&handler, 1) < 0)
    return -1;
  myproc()->alarmticks = ticks;
  myproc()->alarmhandler = handler;
  return 0;
}

int
sys_settickets(void)
{
  int tickets;

  if (argint(0, &tickets) < 0)
    return -1;

  myproc()->tickets = tickets;
  return 0;
}

int sys_getpinfo()
{
  struct pstat *p;

  if (argptr(0, (char **)&p, sizeof(struct pstat *)) < 0) {
    return -1;
  }
  copypinfo(p);
  return 0;
}

int sys_dumppagetable() {
  int pid;

  if (argint(0, &pid) < 0)
    dumppgtab(myproc()->pid);
  else
    dumppgtab(pid);
  return 0;
}

uint sys_getpagetableentry() {
  int pid, addr;

  if (argint(0, &pid) < 0 || argint(1, &addr) < 0) {
    return 0;
  } else {
    return getpte((uint)pid, (uint)addr);
  }
}

int sys_isphysicalpagefree() {
  int ppn;

  if (argint(0, &ppn) < 0) {
    return 0;
  } else {
    return isfree(ppn);
  }
}
