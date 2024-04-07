// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"

void freerange(void *vstart, void *vend);
extern char end[]; // first address after kernel loaded from ELF file
                   // defined by the kernel linker script in kernel.ld

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  int use_lock;
  struct run *freelist;
  char *cow_refbase;
  char *cow_vmbase;
} kmem;

// Initialization happens in two phases.
// 1. main() calls kinit1() while still using entrypgdir to place just
// the pages mapped by entrypgdir on free list.
// 2. main() calls kinit2() with the rest of the physical pages
// after installing a full page table that maps them on all cores.
void
kinit1(void *vstart, void *vend)
{
  cprintf("kinit1 kernel end %p\n", end);
  cprintf("kinit1 %p -> %p\n", vstart, vend);
  initlock(&kmem.lock, "kmem");
  kmem.use_lock = 0;
  freerange(vstart, vend);
}

void
kinit2(void *vstart, void *vend)
{
  cprintf("kinit2 kernel end %p\n", end);
  cprintf("kinit2 %p -> %p\n", vstart, vend);
  char *s = (char*)PGROUNDUP((uint)vstart);
  char *e = (char*)PGROUNDUP((uint)vend);
  uint refs_page_count = (e - s) / PGSIZE;

  kmem.cow_refbase = (char*)vstart;
  // zero the ref count memory
  memset(kmem.cow_refbase, 0, PGSIZE * refs_page_count);

  cprintf("COW REF %d - %p - %p\n", refs_page_count, kmem.cow_refbase, (char*)vstart + refs_page_count);
  vstart = (void*)((char*)vstart + refs_page_count);
  kmem.cow_vmbase = vstart;
  freerange(vstart, vend);
  kmem.use_lock = 1;
}


void
freerange(void *vstart, void *vend)
{
  char *p;
  int pages = 0;
  p = (char*)PGROUNDUP((uint)vstart);
  cprintf("Free range called %p - %p\n", vstart, vend);
  cprintf("First available page %p - %d\n", p, pages);
  for(; p + PGSIZE <= (char*)vend; p += PGSIZE) {
    pages++;
    kfree(p);
  }
  cprintf("Total Available Pages - %d\n", pages);
}
//PAGEBREAK: 21
// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(char *v)
{
  struct run *r;

  if((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(v, 1, PGSIZE);

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = (struct run*)v;
  r->next = kmem.freelist;
  kmem.freelist = r;
  if(kmem.use_lock)
    release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char*
kalloc(void)
{
  struct run *r;

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  if(kmem.use_lock)
    release(&kmem.lock);
  return (char*)r;
}

int isfree(int ppn) {
  uint addr = (uint)P2V(ppn * PGSIZE);
  struct run*t;
  t = kmem.freelist;
  while(t) {
    if (addr == (uint)t)
      return 1;
    else
      t = t->next;
  }
  return 0;
}
