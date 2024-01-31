#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid == -1) {
        printf("fork failed.\n");
    } else if (pid == 0) {
        printf("child proc beginning\n");
        execlp("childtask", NULL);
        printf("child proc complete\n");
    } else {
        printf("parent proc beginning\n");
        int status;
        wait(&status);
        int result = WEXITSTATUS(status);
        printf("parent proc complete\n");
    }

    return 0;
}

