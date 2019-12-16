#include "types.h"
#include "user.h"

#define PGSIZE 4096

#define assert(X)                                                                                                      \
  {                                                                                                                    \
    if (!(X)) {                                                                                                        \
      printf(1, "\nAssertion failed at %s:%d\n", __FILE__, __LINE__);                                                  \
      exit();                                                                                                          \
    }                                                                                                                  \
  }

int main() {
  printf(1, "Testing copy on write...");
  char* mem = malloc(PGSIZE * 2);
  // Init both pages to 0xad
  for (int i = 0; i < PGSIZE * 2; i++) { mem[i] = 0xad; }

  if (fork() == 0) {
    // Set both the the _child's_ pages to 0xae
    for (int i = 0; i < PGSIZE * 2; i++) { mem[i] = 0xae; }

    for (int i = 0; i < PGSIZE * 2; i++) {
      //      printf(1, "childMem[%d] == 0x%x\n", i, mem[i]);
    }
    exit();
  }
  wait();

  // Since the child has its own copy, we know this must still all be 0xad
  for (int i = 0; i < PGSIZE * 2; i++) { assert(mem[i] == 0xad); }

  printf(1, "OK\n");

  exit();
}
