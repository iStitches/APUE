#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <wait.h>

#define BUFFSIZE 1024

int main()
{
    pid_t pid;
    int pipe1[2], pipe2[2];
    pipe(pipe1);
    pipe(pipe2);
    
    pid = fork();
    if (pid == 0) {         //child --> server
        close(pipe1[1]);
        close(pipe2[0]);
        server(pipe1[0], pipe2[1]);
        exit(0);
    } else if (pid > 0) {   //parent --> client
        close(pipe1[0]);
        close(pipe2[1]);
        client(pipe2[0], pipe1[1]);
        waitpid(pid, NULL, 0);
        exit(0);
    }
}

void client(int readfd, int writefd) 
{
    int len, n;
    char buf[BUFFSIZE];
    fgets(buf, BUFFSIZE, stdin);
    len = strlen(buf);
    if (buf[len - 1] == '\n')
        len = len - 1;
    //发送数据
    write(writefd, buf, len);
    //读取数据
    while ((n = read(readfd, buf, len)) > 0) {
        printf("%s\n", buf);
    } 
}

void server(int readfd, int writefd)
{
    int len, n, fd;
    char buf[BUFFSIZE];
    n = read(readfd, buf, BUFFSIZE);
    buf[n] = '\0';
    fd = open(buf, O_RDONLY);       //打开文件失败
    if (fd < 0)
    {
        sprintf(buf + n, "can't open %s\n", strerror(errno));
        write(writefd, buf, n);
    }
    else 
    {
        while ((n = read(fd, buf, BUFFSIZE)) > 0)
        {
            write(writefd, buf, n);
        }
        close(fd);
    }
}