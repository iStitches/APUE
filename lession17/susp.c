#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#define MYREALSIG (SIGRTMIN+6)

static void int_handler(int sig) {
    write(1, "!", 1);
}

int main(int argc, char *argv[]) 
{
    int i, j;
    sigset_t set, oldset, saveset;
        
    signal(MYREALSIG, int_handler);
    sigemptyset(&set);
    sigaddset(&set, MYREALSIG);
    sigprocmask(SIG_UNBLOCK, &set, &saveset);

    sigprocmask(SIG_BLOCK, &set, &oldset);
    for (;;) {
        for (i = 0; i < 5; i++) {
            write(1, "*", 1);
            sleep(1);
        }
        printf("\n");
        sigsuspend(&oldset);
    }
    sigprocmask(SIG_SETMASK, &saveset, NULL);
}