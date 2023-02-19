#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
    匿名管道: 只可用于父子进程
 */

#define BUFFSIZE 1024

int main() 
{
    int fd[2];
    char buf[BUFFSIZE];
    pid_t pid;
    int len;

    if (pipe(fd) < 0) {
        perror("pipe()");
        exit(1);
    }

    pid = fork();
    if (pid == 0) {
        close(fd[1]);
        len = read(fd[0], buf, BUFFSIZE);
        puts(buf);
        close(fd[0]);
        exit(0);
    } else if (pid > 0) {
        close(fd[0]);
        write(fd[1], "Hello!", 6);
        close(fd[1]);
        wait(NULL);
        exit(0);
    }
}