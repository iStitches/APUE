#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFSIZE 1024
#define FILEPATH1 "/tmp/fifo1"
#define FILEPATH2 "/tmp/fifo2"

int main()
{
    int ret;
    ret = access(FILEPATH1, F_OK);
    if (ret == -1) {
        printf("管道1不存在，创建有名管道\n");
        ret = mkfifo(FILEPATH1, 0664);
        if (ret == -1) {
            perror("mkfifo()");
            exit(0);
        }
    }

    ret = access(FILEPATH2, F_OK);
    if (ret == -1) {
        printf("管道2不存在，创建有名管道\n");
        ret = mkfifo(FILEPATH2, 0664);
        if (ret == -1) {
            perror("mkfifo()");
            exit(0);
        }
    }

    int frd, fwd;
    fwd = open(FILEPATH1, O_RDWR);
    if (fwd == -1) {
        perror("open");
        exit(0);
    }
    frd = open(FILEPATH2, O_RDONLY);
    if (frd == -1) {
        perror("open");
        exit(0);
    }

    pid_t pid = fork();
    //父进程写管道1
    if (pid > 0)
    {
        char bufw[128];
        while(1)
        {
            memset(bufw, 0, 128);
            fgets(bufw, 128, stdin);
            int ret2 = write(fwd, bufw, strlen(bufw));
            if (ret2 == -1) {
                perror("write()");
                exit(0);
            }
        }
        close(fwd);
    } else if (pid == 0) {  //子进程读管道2
        char bufr[128];
        while(1)
        {
            memset(bufr, 0, 128);
            int ret3 = read(frd, bufr, 128);
            if (ret3 < 0) {
                perror("read()");
                break;
            }
            int len = strlen(bufr);
            if (len == 0) {
                exit(0);
            }
            printf("buf: %s\n", bufr);
        }
        close(frd);
    }
    return 0;
}