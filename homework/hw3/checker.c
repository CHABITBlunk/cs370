#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  printf("Checker process [%d]: Starting.\n", getpid());

  int fd = atoi(argv[2]);
  int divisor = atoi(argv[0]);
  int dividend = atoi(argv[1]);

  read(fd, &divisor, sizeof(divisor));
  read(fd, &dividend, sizeof(dividend));

  if (!(dividend % divisor)) {
    printf("Checker process [%d]: %d IS divisible by %d.\n", getpid(), dividend, divisor);
    printf("Checker process [%d]: Returning 1.\n", getpid());
    return 1;
  } else {
    printf("Checker process [%d]: %d *IS NOT* divisible by %d.\n", getpid(), dividend, divisor);
    printf("Checker process [%d]: Returning 0.\n", getpid());
    return 0;
  }
}
