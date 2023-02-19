#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "anytimer.h"

struct job {
    int sec;
    int flags;
    at_jobfunc_t *f;
    void *args;
};

struct job *jobs[JOB_MAX];   //任务集合

void f1(void *p) {

}

int main(int argc, char *argv[]) {
    int job1, job2, job3;
    puts("Begin!");
    job1 = at_addjob(5, f1, "aaa");
}