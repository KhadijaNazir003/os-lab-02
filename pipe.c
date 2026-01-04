#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

// Declaration of filter function from reader.c
void filter(int read_fd);

int main() {
    int fd[2];
    pipe(fd);

    pid_t pid = fork();

    if (pid == 0) {
        // Child: start first filter
        close(fd[1]);
        filter(fd[0]);
    } else {
        // Parent: generate numbers 2 to 1000
        close(fd[0]);

        for (int i = 2; i <= 1000; i++) {
            write(fd[1], &i, sizeof(int));
        }

        close(fd[1]);
        wait(NULL);
    }

    return 0;
}

