#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#define FNAME "/home/root/cppProject/lession10/res.txt"

static FILE *fp;

static int daemonize() {
    pid_t pid;
    int fd;
    pid = fork();
    if (pid < 0) {
        return -1;
    }
    if (pid > 0)
        exit(0);
    fd = open("/dev/null", O_RDWR);
    if (fd < 0) {
        return -2;
    }
    dup2(fd, 0);
    dup2(fd, 1);
    dup2(fd, 2);
    if (fd > 2) {
        close(fd);
    }
    setsid();
    chdir("/");
    umask(0);
    return 0;
}

static void daemon_exit(int s) {
    fclose(fp);
    closelog();
    syslog(LOG_INFO, "daemonize_exit");
    exit(0);
}

int main() {
    int i;
    struct sigaction sa;
    //使用signal函数
    // signal(SIGINT, daemon_exit);
    // signal(SIGTERM, daemon_exit);
    // signal(SIGQUIT, daemon_exit);

    //使用sigaction函数
    sa.sa_handler = daemon_exit;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGQUIT);
    sigaddset(&sa.sa_mask, SIGTERM);
    sigaddset(&sa.sa_mask, SIGINT);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    openlog("mydaemon", LOG_PID, LOG_DAEMON);

    //启动守护进程
    if (daemonize()) {
        syslog(LOG_ERR, "daemonize() failed.");
        exit(1);
    } else {
        syslog(LOG_INFO, "daemonize() successed.");
    }
    fp = fopen(FNAME, "w");
    if (fp == NULL) {
        syslog(LOG_ERR, "fopen():%s", strerror(errno));
        exit(1);
    }
    for (i = 0; ;i++) {
        fprintf(fp, "%d\n", i);
        fflush(fp);
        syslog(LOG_DEBUG, "%d was printed.", i);
        sleep(1);
    }
    exit(0);
}