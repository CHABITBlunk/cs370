#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/resource.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc < 5) {
    printf("not enough arguments (required: 5).");
    return -1;
  }
  int shmids[4];
  for (int i = 0; i < 4; i++) {

    shmids[i ] = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);

    int fd[2];
    pipe(fd);

    pid_t pid = fork();

    if (pid < 0) {
      printf("Fork failed.\n");
    }

    else if (pid > 0) {
			close(fd[0]);
			write(fd[1], &shmids[i], sizeof(shmids[i]));
    }

    else {
      printf("Coordinator: forked process with id %d.\n", getpid());
			printf("Coordinator: wrote shm ID %d to pipe (%lu bytes)\n", shmids[i], sizeof(shmids[i]));
      char buffer[8];
      sprintf(buffer, "%d", fd[0]);
      execlp("checker", "checker", buffer, argv[1], argv[i + 2]);
    }
  }

  for (int i = 0; i < 4; i++) {
    // wait for child to finish
    int status;
    wait(&status);

    // attach to & read from shm segment
    int *shmpt = (int*) shmat(shmids[i], NULL, 0);
    if (*shmpt == 1) {
      printf("Coordinator: result %d read from shared memory: %s is divisible by %s.\n", *shmpt, argv[i + 2], argv[1]);
    } else if (*shmpt == 0) {
      printf("Coordinator: result %d read from shared memory: %s is not divisible by %s.\n", *shmpt, argv[i + 2], argv[1]);
    }

    // destroy shmpt
    shmctl(*shmpt, IPC_RMID, NULL);
  }

  printf("Coordinator: exiting.\n");
  return 0;
}
