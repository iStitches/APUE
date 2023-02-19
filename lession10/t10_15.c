#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include "../err.h"

/* 获取调用进程中的信号屏蔽字的信号名 */
void pr_mask(const char* str) {
    sigset_t sigset;
    int errno_save;
    errno_save = errno;
    if (sigprocmask(0, NULL, &sigset) < 0) {
        err_ret("sigprocmask error");
    } else {
        printf("%s", str);
        if (sigismember(&sigset, SIGINT)) {
            printf(" SIGINT");
        }
        if (sigismember(&sigset, SIGQUIT)) {
            printf(" SIGQUIT");
        }
        if (sigismember(&sigset, SIGUSR1)) {
            printf(" SIGUSR1");
        }
        if (sigismember(&sigset, SIGALRM)) {
            printf(" SIGALRM");
        }
        printf("\n");
    }
    errno = errno_save;
}

void sig_int(int signo) {
    pr_mask("\nin sig_int: ");
}

int main(void) {
    sigset_t newmask, oldmask, waitmask;

    /* 设置SIGINT信号处理函数 */
    struct sigaction sa;
    sa.sa_handler = sig_int;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);
    sa.sa_flags = 0;

    pr_mask("program start: ");
    sigaction(SIGINT, &sa, NULL);
    sigemptyset(&waitmask);
    sigaddset(&waitmask, SIGUSR1);
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGINT);

    /* 阻塞SIGINT信号 */
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) {
        err_sys("SIG_BLOCK error");
    }
    pr_mask("in critical region: ");

    /* 屏蔽等待 */
    if(sigsuspend(&waitmask) != -1) {
        err_sys("sigsuspend error");
    }
    pr_mask("after return from sigsuspend:");

    if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) {
        err_sys("SIG_SETMASK error");
    }
    pr_mask("program exit: ");
    exit(0);
}