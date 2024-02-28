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
  for (int i = 2; i < argc; i++) {

    shmids[i - 2] = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);

    int fd[2];
    pipe(fd);

    pid_t pid = fork();

    if (pid < 0) {
      printf("Fork failed.\n");
    }

    else if (pid > 0) {
      printf("Coordinator: forked process with id %d.\n", getpid() + i - 1);
			close(fd[0]);
			write(fd[1], &shmids[i - 2], sizeof(shmids[i - 2]));
			printf("Coordinator: wrote shm ID %d to pipe (%lu bytes)\n", shmids[i - 2], sizeof(shmids[i - 2]));
    }

    else {
      char buffer[8];
      sprintf(buffer, "%d", fd[0]);
      execl("checker", "checker", buffer, argv[1], argv[i]);
    }
  }

  for (int i = 2; i < argc; i++) {
    // wait for child to finish
    int status;
    wait(&status);

    // attach to & read from shm segment
    int *shmpt = (int*) shmat(shmids[i - 2], NULL, 0);
    if (*shmpt == 1) {
      printf("Coordinator: result %d read from shared memory: %s is divisible by %s.\n", *shmpt, argv[i], argv[1]);
    } else if (*shmpt == 0) {
      printf("Coordinator: result %d read from shared memory: %s is not divisible by %s.\n", *shmpt, argv[i], argv[1]);
    }

    // destroy shmpt
    shmctl(*shmpt, IPC_RMID, NULL);
  }

  printf("Coordinator: exiting.\n");
  return 0;
}
