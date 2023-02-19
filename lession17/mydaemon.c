#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <errno.h>
#include <string.h>

#define FNAME "/tmp/out"

static int daemonize(void) {
    pid_t pid;
    int fd;

    pid = fork();
    if (pid < 0) {
        // perror("fork()");
        return -1;
    } else if (pid > 0) {
        exit(1);
    }

    //脱离控制终端
    fd = open("/dev/null", O_RDWR);
    if (fd < 0) {
        // perror("open()");
        return -1;
    }
    dup2(fd, 0);
    dup2(fd, 1);
    dup2(fd, 2);

    //创建会话
    setsid();

    //守护进程执行过程中是否会创建文件，如果创建umask值不能被关掉
    umask(0);

    //为保证守护进程一直运行，切换工作目录到根目录
    chdir("/");
    return 0;
}

int main() {
    FILE *fp;

    //由syslogd去记录系统日志
    openlog("mydaemon", LOG_PID, LOG_DAEMON);
    if (daemonize()) {
        syslog(LOG_ERR, "daemonize() failed!");
        exit(1);
    } else {
        syslog(LOG_INFO, "daemonize() success!");
    }

    fp = fopen(FNAME, "w");
    if (fp == NULL) {
        // perror("fopen()");
        syslog(LOG_ERR, "fopen() :%s", strerror(errno));
        exit(1);
    }
    syslog(LOG_DEBUG, "%s was open", FNAME);
    int i;
    for (i = 1; ; i++) {
        fprintf(fp, "%d\n", i);
        fflush(fp);
        sleep(1);
    }
    return 0;
}