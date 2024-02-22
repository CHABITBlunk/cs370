#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  printf("Checker process [%d]: Starting.\n", getpid());

  int argOne = atoi(argv[0]);
  int argTwo = atoi(argv[1]);

  if (!(argTwo % argOne)) {
    printf("Checker process [%d]: %d IS divisible by %d.\n", getpid(), argTwo, argOne);
    printf("Checker process [%d]: Returning 1.\n", getpid());
    return 1;
  } else {
    printf("Checker process [%d]: %d *IS NOT* divisible by %d.\n", getpid(), argTwo, argOne);
    printf("Checker process [%d]: Returning 0.\n", getpid());
    return 0;
  }
}
