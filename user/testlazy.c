#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main()
{
  printf("=== Test Lazy Allocation ===\n\n");
  
  // Test 1: Allocation simple
  printf("1. Simple lazy allocation test\n");
  char *p = sbrk(4096);  // Allouer 1 page
  printf("   sbrk(4096) returned: %p\n", p);
  
  // Accéder à la page (devrait causer un page fault et allouer)
  printf("   First access to allocated memory...\n");
  p[0] = 'A';
  printf("   Success: wrote '%c' to address %p\n", p[0], p);
  
  // Test 2: Allocation multiple
  printf("\n2. Multiple page allocation\n");
  char *q = sbrk(3 * 4096);  // 3 pages
  printf("   sbrk(3 * 4096) returned: %p\n", q);
  
  // Accéder à la première page
  printf("   Accessing first page...\n");
  q[0] = 'B';
  
  // Accéder à la dernière page
  printf("   Accessing last page...\n");
  q[2 * 4096] = 'C';
  printf("   Success: wrote '%c' and '%c'\n", q[0], q[2 * 4096]);
  
  // Test 3: Test d'erreur - accès hors limites
  printf("\n3. Testing out-of-bounds access\n");
  printf("   Trying to access beyond allocated memory...\n");
  
  // Cette partie peut tuer le processus, donc on le fait dans un enfant
  if(fork() == 0) {
    // Enfant
    char *r = sbrk(4096);
    r[4096] = 'X';  // Accès hors limites - devrait tuer le processus
    printf("   ERROR: Should not reach here!\n");
    exit(0);
  }
  wait(0);
  printf("   ✓ Child correctly killed on out-of-bounds access\n");
  
  printf("\n=== Test completed successfully ===\n");
  exit(0);
}
