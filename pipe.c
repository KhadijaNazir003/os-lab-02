// Anonymous Pipes
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd[2];
    char buf[50];
    pipe(fd);

    if (fork() == 0) { // Child
        close(fd[0]);
        char msg[] = "Hello from child";
        write(fd[1], msg, strlen(msg)+1);
    } else { // Parent
        close(fd[1]);
        read(fd[0], buf, sizeof(buf));
        printf("Parent received: %s\n", buf);
    }
    return 0;
}
