#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Mesurer le temps d'exécution d'une tâche
void measure_task(int task_id, int iterations) {
  int start = uptime();
  
  volatile long sum = 0;
  for(int i = 0; i < iterations; i++) {
    sum += i * i;
  }
  
  int end = uptime();
  printf("Task %d: %d iterations, time = %d ticks, sum = %ld\n", 
       task_id, iterations, end - start, sum);
}

int main() {
  printf("=== Scheduler Performance Test ===\n\n");
  
  // Test 1: Processus unique
  printf("Test 1: Single process\n");
  if(fork() == 0) {
    measure_task(1, 20000000);
    exit(0);
  }
  wait(0);
  
  // Test 2: Deux processus en parallèle
  printf("\nTest 2: Two parallel processes\n");
  int start = uptime();
  
  for(int i = 0; i < 2; i++) {
    if(fork() == 0) {
      measure_task(2 + i, 10000000);
      exit(0);
    }
  }
  
  for(int i = 0; i < 2; i++) {
    wait(0);
  }
  
  int end = uptime();
  printf("Total time for 2 processes: %d ticks\n", end - start);
  
  // Test 3: Trois processus
  printf("\nTest 3: Three parallel processes\n");
  start = uptime();
  
  for(int i = 0; i < 3; i++) {
    if(fork() == 0) {
      measure_task(4 + i, 7000000);
      exit(0);
    }
  }
  
  for(int i = 0; i < 3; i++) {
    wait(0);
  }
  
  end = uptime();
  printf("Total time for 3 processes: %d ticks\n", end - start);
  
  printf("\n=== End of test ===\n");
  exit(0);
}
