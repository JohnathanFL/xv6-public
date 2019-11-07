#include "user.h"

void consume(int t) {t += 1;}

int main(int argc, char** argv) {
  int n = 3;
  for(int k = 0; k < n; k++) {
    int id = fork();
    if(id < 0) {
      printf(1, "Failed to fork!\n");
    } else if (id == 0) {
      //printf(1, "Child checking in\n");
      chpr(getpid(), 20);
      int tmp = 1;
      for(int i = 0; i < 100000000; i++) tmp = i * 1 / 10 * 50;
      consume(tmp);
      break;
    } else {
      //printf(1, "Parent %d creating proc %d\n", getpid(), id);
      wait();
    }
  }


  exit();
}
