#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "../err.h"

static void sig_alrm(int signo) {
    /* 什么都不做,唤醒等待 */
}

unsigned int sleep3(unsigned int seconds) {
    struct sigaction newact, oldact;
    sigset_t newmask, oldmask, suspmask;
    unsigned int unslept;

    /* 设置SIGALRM信号处理函数 */
    newact.sa_handler = sig_alrm;
    sigemptyset(&newact.sa_mask);
    newact.sa_flags = 0;
    sigaction(SIGALRM, &newact, &oldact);

    /* 阻塞SIGALRM信号保存 */
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGALRM);
    sigprocmask(SIG_BLOCK, &newmask, &oldmask);
    
    /* alarm等待超时 */
    alarm(seconds);
    suspmask = oldmask;
    /* 恢复SIGALRM */
    sigdelset(&suspmask, SIGALRM);
    /* 等待新信号 */
    sigsuspend(&suspmask);
    unslept = alarm(0);

    /* 恢复信号状态 */
    sigaction(SIGALRM, &oldact, NULL);
    sigprocmask(SIG_SETMASK, &oldmask, NULL);
    return (unslept);
}

int main() {
    int a = 2;
    sleep3(5);
    printf("%d\n", a);
}