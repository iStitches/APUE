#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <wait.h>
#include <time.h>
#include <unistd.h>

#define MAXIPLEN 16
#define MAXLINE 1024

void str_echo(int sockfd) {
    srand((unsigned)time(NULL));
    int num = rand() % 15;
    printf("num: %d\n", num);
    sleep(num);
    ssize_t n;
    char buf[MAXLINE];

    again:
    while ((n = read(sockfd, buf, MAXLINE)) > 0)
    {
        write(sockfd, buf, n);
        printf("%s\n", buf);
    }
    if(n < 0 && errno == EINTR)
        goto again;
    else if (n < 0) {
        printf("ERROR: read error\n");
    }
}

int showFileList(char *path)
{
    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork()");
        exit(1);
    } else if (pid == 0) {
        if (execlp("ls", "ls", path, (char*) 0) < 0)
        {
            perror("execlp()");
            exit(1);
        }
    } else {
        if (waitpid(pid, NULL, 0) < 0)
        {
            perror("waitpid()");
            exit(1);
        }
    }
}

int main(int argc, char *argv[])
{
    int lfd, cfd, i;
    pid_t pid;
    socklen_t clen;
    struct sockaddr_in s_addr, c_addr;
    int port;
    char addr[MAXIPLEN];

    if (argc <= 1) {
        exit(1);
    }
    
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
    {
        printf("%s\n%s\n\n%s\n%s\n\n%s\n%s\n%s\n%s\n%s\n",
        "Usage: fserver [OPTION] ... <DIR>", "serve some files over tcp", "args:", "    <DIR> Which directory to serve",
        "options:", "    -l, --listen=ADDR       specify source address to use [default is localhost]",
        "    -H, --hidden             show hidden file",
        "    -p, --port=PORT         specify listening port [default port is 12345]",
        "    -h, --help                display this help and exit");
        return 0;
    }
    // for (i = 1; i < argc; i+=2) {
    //     if (strcmp(argv[i], "-p") == 0)
    //         port = atoi(argv[i + 1]);
    //     else if (strcmp(argv[i], "-l") == 0)
    //         strcpy(addr, argv[i + 1]);
    //     else {

    //     }
    // }

    // lfd = socket(AF_INET, SOCK_STREAM, 0);
    // if (lfd < 0)
    // {
    //     perror("socket()");
    //     exit(1);
    // }
    // s_addr.sin_family = AF_INET;
    // s_addr.sin_port = htons(atoi(SERV_PORT));
    // inet_pton(AF_INET, "0.0.0.0", &s_addr.sin_addr.s_addr);

    // if (bind(lfd, (void*)&s_addr, sizeof(s_addr)) < 0)
    // {
    //     perror("bins()");
    //     exit(1);
    // }
    // if (listen(lfd, 200) < 0)
    // {
    //     perror("listen()");
    //     exit(1);
    // }

    // while(1)
    // {
    //     clen = sizeof(c_addr);
    //     cfd = accept(lfd, (void*)&c_addr, &clen);
    //     if ((pid = fork()) == 0) //child
    //     {
    //         close(lfd);
    //         str_echo(cfd);
    //         close(cfd);
    //         exit(0);
    //     } else if(pid > 0) {     //parent
    //         close(cfd);
    //     }
    // }
    return 0;
}