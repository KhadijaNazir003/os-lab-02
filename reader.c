// Named Pipe (FIFO) Reader
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("myfifo", O_RDONLY);
    char buf[50];
    read(fd, buf, sizeof(buf));
    printf("Reader got: %s\n", buf);
    close(fd);
    return 0;
}
