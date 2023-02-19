#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include "../err.h"

int globalVar = 6;
char buf[] = "a write to stdout\n";

int main(void) {
    int var;
    pid_t pid;
    var = 88;

    if (write(STDOUT_FILENO, buf, sizeof(buf) - 1) != sizeof(buf) - 1) {
        err_sys("write error");
    }
    printf("before work\n");    /* 默认采取行缓冲,如果输出到文件采取全缓冲,在fork时缓冲区数据也会复制到子进程 */

    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid == 0) {
        globalVar++;
        var++;
    } else {
        sleep(2);
    }

    printf("pid = %ld, glob = %d, var = %d\n", (long)getpid(), globalVar, var);
    exit(0);
}