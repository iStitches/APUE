#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include "../err.h"

static void pr_exit(int);

int main(void) {
    pid_t pid;
    int status;

    if ((pid = fork()) < 0)
        err_sys("fork error");
    else if (pid == 0)
        exit(7);       /* 正常退出 */

    if (wait(&status) != pid)
        err_sys("wait error!");
    pr_exit(status);

    if ((pid = fork()) < 0)
        err_sys("fork error");
    else if (pid == 0)
        abort();       /* 异常退出 */
    
    if (wait(&status) != pid)
        err_sys("wait error!");
    pr_exit(status);

    if ((pid = fork()) < 0)
        err_sys("wait error");
    else if (pid == 0)
        status /= 0;   /* 异常退出 */
    
    if (wait(&status) != pid)
        err_sys("wait error");
    pr_exit(status);
    exit(0);
}


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