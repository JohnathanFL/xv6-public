#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char** argv) {
  if(argc != 2) {
    printf("Usage: set-prior PID\n");
    exit();
  }
  int pidToCheck = atoi(argv[1]);

  setprior(pidToCheck);

  exit();
  return 0;
}
