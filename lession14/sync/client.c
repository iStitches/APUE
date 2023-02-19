#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <wait.h>

#define SERV_PORT 9877
#define MAXLINE 1024

void str_cli(FILE *fp, int sockfd) {
    char sendline[MAXLINE];
    while (fgets(sendline, MAXLINE, fp) != NULL) {
        write(sockfd, sendline, strlen(sendline));
        fputs(sendline, stdout);
    }
}

int main(int argc, char *argv[]) {
    int sockfd, n;
    char recvline[MAXLINE + 1];

    struct sockaddr_in servaddr;

    /* 创建套接字 */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket()");
        exit(1);
    }

    /* 构建结构体ip、port */
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
    {
        perror("inet_pton()");
        exit(1);
    }

    /* 建立连接 */
    if (connect(sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect()");
        exit(1);
    }
    str_cli(stdin, sockfd);

    return 0;
}