#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "../err.h"

char *env_init[] = { "USER=unknown", "PATH=/home/root/cppProject", NULL};   //自定义环境变量数组

int main(void) {
    pid_t pid;
    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid == 0) {
        if (execle("/home/root/cppProject/t8_15", "t8_15", "arg1", "arg2", (char*) 0, env_init) < 0) {
            err_sys("execle error");
        }
    }

    if (waitpid(pid, NULL, 0) < 0)
        err_sys("wait error");

    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid == 0) {
        if (execlp("t8_15", "t8_15", "only 1 arg", (char*)0) < 0)
            err_sys("execlp error");
    }
    exit(0);
}