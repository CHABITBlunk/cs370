#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {
  printf("Checker process [%d]: Starting.\n", getpid());

  int fd = atoi(argv[1]);
  int divisor = atoi(argv[2]);
  int dividend = atoi(argv[3]);

  int shmid;
  read(fd, &shmid, sizeof(shmid));
  printf("Checker process [%d]: read %lu bytes containing shm ID %d\n", getpid(), sizeof(shmid), shmid);
  int *shmpt = (int*) shmat(shmid, NULL, 0);

  if (!(dividend % divisor)) {
    printf("Checker process [%d]: %d IS divisible by %d.\n", getpid(), dividend, divisor);
    *shmpt = 1;
    printf("Checker process [%d]: wrote result (%d) to shared memory.\n", getpid(), *shmpt);
  } else {
    printf("Checker process [%d]: %d *IS NOT* divisible by %d.\n", getpid(), dividend, divisor);
    *shmpt = 0;
    printf("Checker process [%d]: wrote result (%d) to shared memory.\n", getpid(), *shmpt);
  }
  shmdt(shmpt);
  return 0;
}
