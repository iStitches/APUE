#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

void sigHanlder(int signo) {
    struct timeval tm;
    gettimeofday(&tm, NULL);

    switch (signo) {
        case SIGALRM:
            printf("Get the SIGALRM signal! time = %ld.%03ld\n",
                tm.tv_sec, tm.tv_usec/1000);
            break;
        case SIGVTALRM:
            printf("Get the SIGVTALRM signal! time = %ld.%03ld\n",
                tm.tv_sec, tm.tv_usec / 1000);
            break;
        case SIGPROF:
            printf("Get the SIGPROF signal! time = %ld.%03ld\n",
                tm.tv_sec, tm.tv_usec / 1000);
            break;
    }
}

int main() {
    struct itimerval new_value = {0};
    signal(SIGALRM, sigHanlder);

    //启动定时器时间
    new_value.it_value.tv_sec = 2;
    new_value.it_value.tv_usec = 0;

    //定时器间隔时间
    new_value.it_interval.tv_sec = 3;
    new_value.it_interval.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &new_value, NULL) < 0) {
        printf("Setitimer Failed : %s\n", strerror(errno));
        _exit(EXIT_FAILURE);
    }

    while (1) {
        pause();
    }
    return 0;
}