#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/resource.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  for (int i = 2; i < argc; i++) {
    
    int shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);

    int fd[2];
    pipe(fd);

    pid_t pid = fork();

    if (pid == -1) {
      printf("Fork failed.\n");
    }
    else if (pid == 0) {
      printf("Coordinator: forked process with id %d.\n", getpid());
			close(fd[0]);
			write(fd[1], &shmid, sizeof(shmid));
			printf("Coordinator: wrote shm ID %d to pipe (%d bytes)", shmid, sizeof(shmid));
      execl("checker", NULL, fd, argv[1], argv[i]);
    }
    else {
      // wait for child to finish
      int status;
      wait(&status);
      
      // attach to & read from shm segment
      int *shmpt = (int*) shmat(shmid, NULL, 0);

      // destroy shmpt
      shmctl(*shmpt, IPC_RMID, NULL);
    }
  }

  printf("Coordinator: exiting.\n");
  return 0;
}
