#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#define BUFSIZE 10

static volatile int loop = 0;

static void alarm_handler(int sig) {
    alarm(1);
    loop = 0;
}

int main(int argc, char *argv[]) {
    int fd;
    ssize_t readsize = -1;
    ssize_t writesize = -1;
    ssize_t off = 0;
    char buffer[BUFSIZE] = "";

    do {
        fd = open(argv[1], O_RDONLY);
        if (fd < 0) {
            if (EINTR != errno) {
                perror("open()");
                return 1;
            }
        }
    } while (fd < 0);

    loop = 1;
    signal(SIGALRM, alarm_handler);
    alarm(1);
    while (1) {
        while (loop)
            pause();
        loop = 1;

        while ((readsize = read(fd, buffer, BUFSIZE)) < 0) {
            if (readsize < 0) {
                if (EINTR == errno) {
                    continue;
                } else {
                    perror("read()");
                    close(fd);
                }
            }
        }

        //读完
        if (!readsize) {
            break;
        }

        off = 0;
        do {
            writesize = write(1, buffer + off, readsize);
            off += writesize;
            readsize -= writesize;
        } while (readsize > 0);
    }
    close(fd);
    return 0;
}