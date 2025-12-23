#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main()
{
  printf("=== Test getprocinfo ===\n");
  
  // Test simple - notre propre processus
  int mypid = getpid();
  printf("Testing current process (PID %d)\n", mypid);
  
  int pid, state, ticks, syscalls;
  
  // Appel système avec récupération de tous les registres
  asm volatile(
    "li a7, 22\n"     // SYS_getprocinfo = 22
    "mv a0, %4\n"     // pid argument
    "ecall\n"         // syscall
    "mv %0, a0\n"     // PID
    "mv %1, a1\n"     // State
    "mv %2, a2\n"     // Ticks used
    "mv %3, a3\n"     // Syscall count
    : "=r" (pid), "=r" (state), "=r" (ticks), "=r" (syscalls)
    : "r" (mypid)
    : "a0", "a1", "a2", "a3", "a7"
  );
  
  printf("Results:\n");
  printf("  PID: %d\n", pid);
  printf("  State: %d ", state);
  
  // Décoder l'état
  switch(state) {
    case 0: printf("(UNUSED)\n"); break;
    case 1: printf("(USED)\n"); break;
    case 2: printf("(SLEEPING)\n"); break;
    case 3: printf("(RUNNABLE)\n"); break;
    case 4: printf("(RUNNING)\n"); break;
    case 5: printf("(ZOMBIE)\n"); break;
    default: printf("(UNKNOWN)\n");
  }
  
  printf("  Ticks used: %d\n", ticks);
  printf("  Syscall count: %d\n", syscalls);
  
  // Test processus init (PID 1)
  printf("\nTesting init process (PID 1):\n");
  
  asm volatile(
    "li a7, 22\n"
    "li a0, 1\n"
    "ecall\n"
    "mv %0, a0\n"
    "mv %1, a1\n"
    : "=r" (pid), "=r" (state)
    : 
    : "a0", "a1", "a7"
  );
  
  if(pid == 1) {
    printf("  Found init process\n");
    printf("  State: %d\n", state);
  } else {
    printf("  Error: Returned %d\n", pid);
  }
  
  printf("\n=== Test completed ===\n");
  
  exit(0);
}
