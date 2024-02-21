#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  for (int i = 2; i < argc; i++) {
    pid_t pid = fork();
    int pid_number = getpid();
    if (pid == -1) {
      printf("Fork failed.\n");
    } else if (pid == 0) {
      printf("Coordinator: forked process with id %d.\n", pid_number);
      printf("Coordinator: waiting for process [%d].\n", pid_number);
      execlp("checker", argv[1], argv[i]);
    } else {
      int status;
      wait(&status);
      int result = WEXITSTATUS(status);
      printf("Coordinator: child process %d returned %d.\n", pid_number, result);
    }
  }
  printf("Coordinator: exiting.\n");
  return 0;
}
