// POSIX Shared Memory + Semaphore Writer
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd = shm_open("/myshm", O_CREAT | O_RDWR, 0666);
    ftruncate(fd, 1024);
    char *ptr = (char *)mmap(0, 1024, PROT_WRITE, MAP_SHARED, fd, 0);

    sem_t *sem = sem_open("/mysem", O_CREAT, 0666, 0);
    strcpy(ptr, "Hello Shared Memory");
    sem_post(sem);

    munmap(ptr, 1024);
    close(fd);
    sem_close(sem);
    return 0;
}
