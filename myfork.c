#include "user.h"


int main(int argc, char** argv) {
  // Honestly not sure what N is supposed to do in this assignment
  int n = atoi(argv[1]);
  for (int k = 0; k < n; k++) {
    int id = fork();
    if (id < 0) {
      printf(1, "Failed to fork!\n");
    } else if (id == 0) {
      // printf(1, "Child checking in\n");
      int tmp = 1;
      for (int i = 0; i < 1000000000; i++) tmp = i * getpid() / 10 * 50;

      printf(1, "%d\n", tmp);
      break;
    } else {
      // printf(1, "Parent %d creating proc %d\n", getpid(), id);
      wait();
    }
  }


  exit();
}
