#include "types.h"
#include "stat.h"
#include "user.h"


int main() {
  chpr(getpid(), 20);
  for (int i = 0; i < 10000; i++)
    ;

  hello(5);
  exit();
}
