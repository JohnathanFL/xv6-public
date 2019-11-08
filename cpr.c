#include "user.h"

int main(int argc, char* argv[]) {

  if (argc < 3) {
    printf(1, "Usage: cpr <PID> <NEWPRIOR>\n");
    exit();
  }

  int pid      = atoi(argv[1]);
  int newPrior = atoi(argv[2]);

  chpr(pid, newPrior);

  exit();
}
