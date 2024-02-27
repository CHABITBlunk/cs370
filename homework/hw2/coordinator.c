#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	int child_pid;
  for (int i = 2; i < argc; i++) {
    pid_t pid = fork();
		child_pid = getpid() + i - 1;
    if (pid == -1) {
      printf("Fork failed.\n");
    } else if (pid == 0) {
			child_pid = getpid();
      printf("Coordinator: forked process with id %d.\n", child_pid);
      printf("Coordinator: waiting for process [%d].\n", child_pid);
      execl("checker", argv[1], argv[i]);
    } else {
      int status;
      wait(&status);
      int result = WEXITSTATUS(status);
      printf("Coordinator: child process %d returned %d.\n", child_pid, result);
    }
  }
  printf("Coordinator: exiting.\n");
  return 0;
}
