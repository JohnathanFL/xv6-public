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
extern uint     vectors[];  // in vectors.S: array of 256 entry pointers
struct spinlock tickslock;
uint            ticks;

void tvinit(void) {
  int i;

  for (i = 0; i < 256; i++) SETGATE(idt[i], 0, SEG_KCODE << 3, vectors[i], 0);
  SETGATE(idt[T_SYSCALL], 1, SEG_KCODE << 3, vectors[T_SYSCALL], DPL_USER);

  initlock(&tickslock, "time");
}

void idtinit(void) { lidt(idt, sizeof(idt)); }

// PAGEBREAK: 41
void trap(struct trapframe* tf) {
  if (tf->trapno == T_SYSCALL) {
    if (myproc()->killed) exit();
    myproc()->tf = tf;
    syscall();
    if (myproc()->killed) exit();
    return;
  }

  switch (tf->trapno) {
  case T_IRQ0 + IRQ_TIMER:
    if (cpuid() == 0) {
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
  case T_IRQ0 + IRQ_IDE + 1:
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
    cprintf("cpu%d: spurious interrupt at %x:%x\n", cpuid(), tf->cs, tf->eip);
    lapiceoi();
    break;
case T_PGFLT:
   ; 
    //cprintf("Handling a pagefault\n");
    uint faultAddr = rcr2();
    //// Per here: https://wiki.osdev.org/Paging#Page_Faults, bit 0 is the "present in page table" flag.
    //// Thus if it's not set, we're handling a genuine page fault we need to map.
    //// PTE_P is the "is this address present" bit in the page table itself.
    if(!(tf->err & 1)) {
      if (faultAddr > myproc()->sz) {
        cprintf("SEGfault\n");
        myproc()->killed = 1;
      } else {
        //// Per the same article, CR2 contains the address that caused the fault.
        handle_pgflt(faultAddr);
      }
    } else {
      //cprintf("Page fault where page was present!\n");
      //cprintf("faultAddr: %d, err: %d\n", faultAddr, tf->err);
      //// bit 2 is the "were we trying to write to this? flag
      if(tf->err & 2) {
        // TODO: Check to make sure the write is in user accessible stuff
        handle_cow_pgflt(faultAddr);
        break;
      }
      //// If we're here, we were trying to read from some memory
      if (tf->err & 4) { //// Per same article, bit 4 is the "is this a user proc" flag
        //// We'll assume that if we tried to read from a present pte as a user, we were being wary wary bad wittle wabbits.
        cprintf("You've been very naughty, PID%d\n", myproc()->pid);
        myproc()->killed = 1;
      } else {
        cprintf("Kernel did something wrong.\n");
        panic("pagefault");
      }
    }
    break;
  // PAGEBREAK: 13
  default:
    if (myproc() == 0 || (tf->cs & 3) == 0) {
      // In kernel, it must be our mistake.
      cprintf("unexpected trap %d from cpu %d eip %x (cr2=0x%x)\n", tf->trapno, cpuid(), tf->eip, rcr2());
      panic("trap");
    }
    // In user space, assume process misbehaved.
    cprintf(
        "pid %d %s: trap %d err %d on cpu %d "
        "eip 0x%x addr 0x%x--kill proc\n",
        myproc()->pid, myproc()->name, tf->trapno, tf->err, cpuid(), tf->eip, rcr2());
    myproc()->killed = 1;
  }

  // Force process exit if it has been killed and is in user space.
  // (If it is still executing in the kernel, let it keep running
  // until it gets to the regular system call return.)
  if (myproc() && myproc()->killed && (tf->cs & 3) == DPL_USER) exit();

  // Force process to give up CPU on clock tick.
  // If interrupts were on while locks held, would need to check nlock.
  if (myproc() && myproc()->state == RUNNING && tf->trapno == T_IRQ0 + IRQ_TIMER) yield();

  // Check if the process has been killed since we yielded
  if (myproc() && myproc()->killed && (tf->cs & 3) == DPL_USER) exit();
}
