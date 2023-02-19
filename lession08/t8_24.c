#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include "../err.h"

static void pr_exit(int status) {
    if (WIFEXITED(status)) {
        printf("normal terminated, exit status = %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status))
        printf("abnormal terminated, signal number = %d%s\n", WTERMSIG(status), 
    #ifdef WCOREDUMP
        WCOREDUMP(status) ? "(core file generated)" : "");
    #else
        "");
    #endif
    else if (WIFSTOPPED(status)) 
        printf("child stopped, signal number = %d\n", WSTOPSIG(status));
}

int main(int argc, char *argv[]) {
    int status;
    if (argc < 2)
        err_exit("command args error!", errno);
    if ((status = system(argv[1])) < 0)
        err_sys("system() error");
    pr_exit(status);
    exit(0);
}