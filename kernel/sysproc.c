#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"
#include "activity.h"
#include "procinfo.h"
#include "syscall.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  
  struct proc *p = myproc();
  addr = p->sz;
  
  // LAZY ALLOCATION: ne pas allouer physiquement tout de suite
  if(n > 0) {
    // Juste augmenter la taille virtuelle
    p->sz += n;
  } else if(n < 0) {
    // Pour la réduction, on peut libérer
    p->sz = uvmdealloc(p->pagetable, p->sz, p->sz + n);
  }
  
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_getactivity(void)
{
  struct activity a;
  uint64 uaddr;

  static uint last_ticks = 0;
  static uint last_active = 0;

  argaddr(0, &uaddr);
  if(uaddr == 0)
    return -1;

  // temps actuel
  uint now = ticks;

  // somme activité CPU de tous les harts
  uint active = 0;
  for(int i = 0; i < NCPU; i++){
    active += cpus[i].ticks_active;
  }

  uint dt = now - last_ticks;
  uint da = active - last_active;

  if(dt > 0)
    a.cpu_percent = (da * 100) / dt;
  else
    a.cpu_percent = 0;

  // champs simples (pour l’instant)
  a.mem_percent = 0;
  a.user_activity = 0;
  a.interrupts = 0;
  a.state = 0;
  safestrcpy(a.timestamp, "RUN", sizeof(a.timestamp));

  last_ticks = now;
  last_active = active;

  if(copyout(myproc()->pagetable, uaddr, (char*)&a, sizeof(a)) < 0)
    return -1;

  return 0;
}

uint64
sys_getprocinfo(void)
{
  int pid;
  struct proc *p;
  
  // Récupérer le PID - argint() ne retourne PAS de valeur, c'est void!
  argint(0, &pid);  // SIMPLE ASSIGNMENT, NO CHECK!
  
  // Chercher le processus
  for(p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    if(p->pid == pid) {
      // Processus trouvé
      struct proc *current = myproc();
      current->trapframe->a0 = p->pid;
      current->trapframe->a1 = p->state;
      current->trapframe->a2 = p->ticks_used;
      current->trapframe->a3 = p->syscall_count;
      release(&p->lock);
      return 0;
    }
    release(&p->lock);
  }
  
  return -1;
}
