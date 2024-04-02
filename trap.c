
#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "traps.h"
#include "spinlock.h"

// Interrupt descriptor table (shared by all CPUs).
struct gatedesc idt[256];
extern uint vectors[];  // in vectors.S: array of 256 entry pointers
struct spinlock tickslock;
uint ticks;

int mappages(pde_t *pgdir, void *va, uint size, uint pa, int perm);
pte_t *walkpgdir(pde_t *pgdir, const void *va, int alloc);

void
tvinit(void)
{
  int i;

  for(i = 0; i < 256; i++)
    SETGATE(idt[i], 0, SEG_KCODE<<3, vectors[i], 0);
  SETGATE(idt[T_SYSCALL], 1, SEG_KCODE<<3, vectors[T_SYSCALL], DPL_USER);

  initlock(&tickslock, "time");
}

void
idtinit(void)
{
  lidt(idt, sizeof(idt));
}

//PAGEBREAK: 41
void
trap(struct trapframe *tf)
{
  if(tf->trapno == T_SYSCALL){
    if(myproc()->killed)
      exit();
    myproc()->tf = tf;
    syscall();
    if(myproc()->killed)
      exit();
    return;
  }

  switch(tf->trapno){
  case T_IRQ0 + IRQ_TIMER:
    if(cpuid() == 0){
      acquire(&tickslock);
      ticks++;
      wakeup(&ticks);
      release(&tickslock);
    }
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE:
    ideintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE+1:
    // Bochs generates spurious IDE1 interrupts.
    break;
  case T_IRQ0 + IRQ_KBD:
    kbdintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_COM1:
    uartintr();
    lapiceoi();
    break;
  case T_IRQ0 + 7:
  case T_IRQ0 + IRQ_SPURIOUS:
    cprintf("cpu%d: spurious interrupt at %x:%x\n",
            cpuid(), tf->cs, tf->eip);
    lapiceoi();
    break;

  //PAGEBREAK: 13
  default:
    pde_t *faulting_entry;
    faulting_entry = walkpgdir(myproc()->pgdir, (const void *)rcr2(), 0);
    cprintf("Faulting entry %p\n", faulting_entry);
    cprintf("pid %d %s: trap %d err %d on cpu %d "
            "eip 0x%x addr 0x%x --kill proc\n",
            myproc()->pid, myproc()->name, tf->trapno,
            tf->err, cpuid(), tf->eip, rcr2());
    // myproc() is zero there is some problem in kernel
    // because init process is always running.
    if (myproc() == 0)
      goto kernel_panic;

    // Kernel mode - Handle Lazy Allocation and COW
    // if pagefault with read/write error code on unmapped memory; allocate page
    // otherwise kernel panic
    else if ((tf->cs&3) == 0)
      if (tf->trapno == T_PGFLT && (tf->err == 0 || tf->err == 2))
        goto allocate_page;
      else if (tf->trapno == T_PGFLT && tf->err == 3)
        goto handle_cow;
      else
        goto kernel_panic;

    // User mode - Handle Lazy Allocation and COW
    // If pagefault with read/write error code on unmapped memory; allocate page
    // otherwise kill user process
    else
      if (tf->trapno == T_PGFLT && (tf->err == 4 || tf->err == 6))
        goto allocate_page;
      else if (tf->trapno == T_PGFLT && tf->err == 7)
        goto handle_cow;
      else
        goto kill_process;

  handle_cow:
    pte_t *entry, *re_entry;
    char *old_phys_page, *new_phys_page;

    entry = walkpgdir(myproc()->pgdir, (const void *)rcr2(), 0);
    old_phys_page = (char *)PTE_ADDR(*entry);

    cprintf("COW Write Access Request %s - %p - %p\n", myproc()->name, rcr2(), tf->eip);
    cprintf("COW Flag - %p\n", *entry&PTE_COW);
    cprintf("COW Page Entry - %p\n", *entry);
    cprintf("COW Old Physical Page - %p\n", old_phys_page);


    if (*entry&PTE_COW) {
      new_phys_page = kalloc();
      cprintf("COW New Phys Page %p\n", new_phys_page);
      if (new_phys_page == 0) {
        cprintf("allocuvm out of memory\n");
        goto kill_process;
      }
      *entry = 0x0;
      if (mappages(myproc()->pgdir, (void *)PGROUNDDOWN(rcr2()), PGSIZE, V2P(new_phys_page), PTE_W|PTE_U) < 0) {
        cprintf("allocuvm out of memory (2)\n");
        kfree(new_phys_page);
        goto kill_process;
      }
      cprintf("Is memmove throwing trap\n");
      cprintf("COW handling done new address - %p, old address - %p\n", new_phys_page, old_phys_page);
      memmove(new_phys_page, P2V(old_phys_page), PGSIZE);
      re_entry = walkpgdir(myproc()->pgdir, (void *)rcr2(), 0);
      cprintf("COW Handled - %p\n", *re_entry);
      break;
    }
    else {
      goto kill_process;
    }

  allocate_page:
    char *pa = kalloc();
    cprintf("Lazy Mem %p\n", pa);
    if(pa == 0){
      cprintf("allocuvm out of memory\n");
      goto kill_process;
    }
    memset(pa, 0, PGSIZE);
    cprintf("allocating va %p\n", PGROUNDDOWN(rcr2()));
    if(mappages(myproc()->pgdir, (char*)PGROUNDDOWN(rcr2()), PGSIZE, V2P(pa), PTE_W|PTE_U) < 0){
      cprintf("allocuvm out of memory (2)\n");
      kfree(pa);
      goto kill_process;
    }
    break;

  kill_process:
    cprintf("KILL KILLL KILLL KILLLLLLL\n");
    /*cprintf("pid %d %s: trap %d err %d on cpu %d "
            "eip 0x%x addr 0x%x--kill proc\n",
            myproc()->pid, myproc()->name, tf->trapno,
            tf->err, cpuid(), tf->eip, rcr2());*/
    myproc()->killed = 1;
    break;
  kernel_panic:
    cprintf("Kernel panic ho hi gya\n");
    cprintf("unexpected trap %d err %d from cpu %d eip %x (cr2=0x%x)\n",
              tf->trapno, tf->err, cpuid(), tf->eip, rcr2());
    panic("trap");
    break;
  }

  // Force process exit if it has been killed and is in user space.
  // (If it is still executing in the kernel, let it keep running
  // until it gets to the regular system call return.)
  if(myproc() && myproc()->killed && (tf->cs&3) == DPL_USER)
    exit();

  // Force process to give up CPU on clock tick.
  // If interrupts were on while locks held, would need to check nlock.
  if(myproc() && myproc()->state == RUNNING &&
     tf->trapno == T_IRQ0+IRQ_TIMER)
    yield();

  // Check if the process has been killed since we yielded
  if(myproc() && myproc()->killed && (tf->cs&3) == DPL_USER)
    exit();
}
