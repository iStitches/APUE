#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "mytbf.h"

#define CPS 10
#define BUFSIZE 1024
#define BURST 100

int main(int argc, char *argv[]) {
    int fd = -1;
    char buf[BUFSIZE] = "";
    ssize_t readsize = -1;
    ssize_t writesize = -1;
    size_t off = 0;
    ssize_t size = 0;
    mytbf_t *tbf;
    
    if (argc < 2) {
        fprintf(stderr, "Usage error");
        return 1;
    }

    tbf = mytbf_init(CPS, BURST);

    do {
        fd = open(argv[1], O_RDONLY);
        if (fd < 0) {
            if (EINTR != errno) {
                perror("open()");
                return 1;
            }
        }
    } while (fd < 0);

    while (1) {
        size = mytbf_fetchtoken(tbf, BUFSIZE);

        while ((readsize = read(fd, buf, size)) < 0) {
            if (readsize < 0) {
                if (errno == EINTR) {
                    continue;
                }
                perror("read()");
                close(fd);
            }
        }
        if (!readsize) {
            break;
        }
        if (size - readsize > 0)
            mytbf_returntoken(tbf, size - readsize);

        off = 0;
        do {
            writesize = write(1, buf + off, readsize);
            off += writesize;
            readsize -= writesize;
        } while (readsize > 0);
    }

    close(fd);
    mytbf_destory(tbf);
    return 0;
}