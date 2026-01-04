#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

/*
 * Each filter:
 * 1. Reads first number -> prime
 * 2. Prints prime
 * 3. Creates a new pipe
 * 4. Forks next filter
 * 5. Sends non-multiples to next filter
 */

void filter(int read_fd) {
    int prime;

    // Read first number (prime)
    if (read(read_fd, &prime, sizeof(int)) <= 0) {
        close(read_fd);
        exit(0);
    }

    printf("%d ", prime);
    fflush(stdout);

    int fd[2];
    pipe(fd);

    pid_t pid = fork();

    if (pid == 0) {
        // Child: next filter
        close(fd[1]);
        close(read_fd);
        filter(fd[0]);
    } else {
        // Parent: filter numbers
        close(fd[0]);

        int num;
        while (read(read_fd, &num, sizeof(int)) > 0) {
            if (num % prime != 0) {
                write(fd[1], &num, sizeof(int));
            }
        }

        close(read_fd);
        close(fd[1]);
        wait(NULL);
        exit(0);
    }
}
