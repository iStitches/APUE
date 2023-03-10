#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include "proto.h"

#define BUFSIZE 1024
#define IPSTRSIZE 40

static void server_job(int sd)
{
    char buf[BUFSIZE];
    memset(buf, 0, BUFSIZE);
    int len;
    len = sprintf(buf, FMT_STAMP, (long long)time(NULL));
    if (send(sd, buf, len, 0) < 0)
    {
        perror("send()");
        exit(1);
    }
    return ;
}

int main() 
{
    int sd, newsd;
    struct sockaddr_in laddr, raddr;
    socklen_t raddr_len;
    char ipstr[IPSTRSIZE];

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0)
    {
        perror("socket()");
        exit(1);
    }

    //设置套接字选项
    int val = 1;
    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0)
    {
        perror("setsockopt()");
        exit(1);
    }
    laddr.sin_family = AF_INET;
    laddr.sin_port = htons(atoi(SERVERPORT));
    inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr.s_addr);

    //绑定端口
    if (bind(sd, (void*)&laddr, sizeof(laddr)) < 0)
    {
        perror("bind()");
        exit(1);
    }
    //开始监听
    if (listen(sd, 200) < 0)
    {
        perror("listen()");
        exit(1);
    }

    raddr_len = sizeof(raddr);

    while(1)
    {
        newsd = accept(sd, (void*)&raddr, &raddr_len);
        if (newsd < 0) {
            if (errno == EINTR || errno == EAGAIN)
                continue;
            perror("newsd()");
            exit(1);
        }
        inet_ntop(AF_INET, &raddr.sin_addr, ipstr, IPSTRSIZE);
        printf("Client:%s:%d\n", ipstr, ntohs(raddr.sin_port));
        server_job(newsd);
        close(newsd);
    }
    close(sd);
    exit(0);
}