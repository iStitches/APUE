#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "../err.h"

int main(void) {
    pid_t pid;
    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid == 0) {          /* first fork Process B */
        if ((pid = fork()) < 0) {
            err_sys("fork error");
        } else if (pid > 0) {       /* Process B */
            exit(0);
        }
        sleep(2);
        printf("second child, parent pid = %ld\n", (long)getppid());
        exit(0);
    }

    if (waitpid(pid, NULL, 0) != pid) /* wait for Process B */
        err_sys("waitpid error");
    
    exit(0);
}