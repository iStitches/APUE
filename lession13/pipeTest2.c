#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define PATHNAME "/tmp/myfifo"
#define BUFFSIZE 1024

int main(void)
{
    pid_t pid;
    int fd = -1;
    char buf[BUFFSIZE] = "";

    if (mkfifo(PATHNAME, 0644) < 0) {
        perror("mkfifo()");
        exit(1);
    }

    fflush(NULL);
    pid = fork();
    if (pid < 0) {
        perror("fork()");
        exit(1);
    }
    if (pid > 0) {                    //parent
        pid = fork();
        if (pid < 0) {
            perror("fork()");
            exit(1);
        }
        if (!pid) {                   //父进程退出,让两个子进程没有亲缘关系
            exit(0);
        }
        fd = open(PATHNAME, O_RDWR);  //子进程
        if (fd < 0) {
            perror("open()");
            exit(1);
        }
        read(fd, buf, BUFFSIZE);
        printf("%s", buf);
        write(fd, "World!", 8);
        close(fd);
        exit(0);
    } else {                          //子进程
        fd = open(PATHNAME, O_RDWR);
        if (fd < 0) {
            perror("open()");
            exit(1);
        }
        write(fd, "Hello", 6);
        sleep(1);
        read(fd, buf, BUFFSIZE);
        close(fd);
        puts(buf);
        //进程退出,把管道文件删除
        remove(PATHNAME);
        exit(0);
    }
    return 0;
}