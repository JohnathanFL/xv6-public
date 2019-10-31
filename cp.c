#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

char buf[512];

int main(int argc, char** argv) {
  int fd[2], n;

  if (argc <= 2) {
    printf(1, "Usage: cp <file> <dest>\n");
    exit();
  }

  if ( (fd[0] = open(argv[1], O_RDONLY)) < 0) {
    printf(1, "cp: cannot open %s\n", argv[1]);
    exit();
  }

  if ( (fd[1] = open(argv[2], O_CREATE|O_RDWR)) < 0) {
    printf(1, "cp: cannot open %s\n", argv[2]);
    exit();
  }

  while ( (n = read(fd[0], buf, sizeof(buf))) ) {
    write(fd[1], buf, n);
  }

  close(fd[0]);
  close(fd[1]);
  
  return 0;
}
