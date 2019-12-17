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
  printf(1, "Testing copy on write...\n");
  int* mem = malloc(PGSIZE);
  // Init both pages to 0xad
  for (int i = 0; i < PGSIZE; i++) { mem[i] = 0xad; }

  if (fork() == 0) {
    // Set both the the _child's_ pages to 0xae
    for (int i = 0; i < PGSIZE; i++) { mem[i] = 0xae; }

    for (int i = 0; i < PGSIZE; i++) {
      //      printf(1, "childMem[%d] == 0x%x\n", i, mem[i]);
    }
    exit();
  }
  wait();

  // Since the child has its own copy, we know this must still all be 0xad
  for (int i = 0; i < PGSIZE; i++) { assert(mem[i] == 0xad); }

  printf(1, "\n\nOK\n\n");

  exit();
}
