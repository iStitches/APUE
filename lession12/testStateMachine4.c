#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/epoll.h>

/*
    需求：从设备 tty11 读取输入并输出到 tty12 上，同样从 tyy12 读取输入并输出到 tty11 上
 */

#define BUFFSIZE 10
#define TTY1 "/home/root/cppProject/lession12/a.txt"
#define TTY2 "/home/root/cppProject/lession12/b.txt"

enum {
    STATE_R=1,
    STATE_W,
    STATE_AUTO,
    STATE_EX,
    STATE_T
};

struct fsm_st {
    int state;              //状态机状态
    int sfd;                //源文件描述符
    int ofd;                //目的文件描述符
    char buf[BUFFSIZE];     //缓冲区
    int len;                //一次读取的字节数
    int pos;                //每次写入数据时buf的偏移量
    char *errstr;           //异常信息
};

//状态机驱动
static void fsm_driver(struct fsm_st *fsm) {
    int ret;
    switch(fsm->state) {
        case STATE_R:  //读状态
            fsm->len = read(fsm->sfd, fsm->buf, BUFFSIZE);
            printf("fsm->len: %d\n", fsm->len);
            if (fsm->len == 0)
                fsm->state = STATE_T;
            else if (fsm->len < 0) 
            {
                if (errno == EAGAIN)        //读状态假错,重新读一次
                    fsm->state = STATE_R;
                else {
                    fsm->state = STATE_EX;
                    fsm->errstr = "read()";
                }
            }
            else {
                fsm->pos = 0;
                fsm->state = STATE_W;
            }
            break;
        
        case STATE_W:  //写状态
            ret = write(fsm->ofd, fsm->buf+fsm->pos, fsm->len);
            if (ret < 0) {
                if (errno == EAGAIN) {
                    fsm->state = STATE_W;
                } else {
                    fsm->errstr = STATE_EX;
                    fsm->errstr = "write()";
                }
            } else {
                fsm->pos += ret;
                fsm->len -= ret;
                if (fsm->len == 0) 
                    fsm->state = STATE_R;
                else
                    fsm->state = STATE_W;
            }
            break;
        
        case STATE_EX:  //异常状态
            perror(fsm->errstr);
            fsm->state = STATE_T;
            break;
        
        case STATE_T:   //结束状态
            printf("状态机结束!\n");
            break;
        
        default:
            abort();
    }
}

//推送状态机
static void relay(int fd1, int fd2) {
    int fd1_save, fd2_save;
    struct fsm_st fsm12, fsm21;
    int epfd;
    struct epoll_event ev;

    epfd = epoll_create(10);
    if (epfd < 0) {
        perror("epfd()");
        exit(1);
    }

    //调整为非阻塞IO
    fd1_save = fcntl(fd1, F_GETFL);
    fcntl(fd1, F_SETFL, fd1_save | O_NONBLOCK);
    fd2_save = fcntl(fd2, F_GETFL);
    fcntl(fd2, F_SETFL, fd2_save | O_NONBLOCK);
    
    //设置状态机初态
    fsm12.state = STATE_R;
    fsm12.sfd = fd1;
    fsm12.ofd = fd2;
    fsm21.state = STATE_R;
    fsm21.sfd = fd2;
    fsm21.ofd = fd1;

    ev.events = 0;
    ev.data.fd = fd1;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd1, &ev);
    
    ev.events = 0;
    ev.data.fd = fd2;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd2, &ev);

    //推动状态机
    while(fsm12.state != STATE_T || fsm21.state != STATE_T)
    {
        //布置监视任务
        ev.events = 0;
        ev.data.fd = fd1;
        if (fsm12.state == STATE_R)
            ev.events |= EPOLLIN;
        if (fsm12.state == STATE_W)
            ev.events |= EPOLLOUT;
        epoll_ctl(epfd, EPOLL_CTL_MOD, fd1, &ev);

        ev.events = 0;
        ev.data.fd = fd2;
        if (fsm21.state == STATE_R)
            ev.events |= EPOLLIN;
        if (fsm21.state == STATE_W)
            ev.events |= EPOLLOUT;
        epoll_ctl(epfd, EPOLL_CTL_MOD, fd2, &ev);

        if (fsm12.state < STATE_AUTO || fsm21.state < STATE_AUTO)
        {
            while (epoll_wait(epfd, &ev, 1, -1) < 0)  //阻塞监视
            {
                if (errno == EINTR)
                    continue;
                perror("epoll_wait()");
                exit(1);
            }
        }

        if (ev.data.fd == fd1 && ev.events & EPOLLIN || ev.data.fd == fd2 && ev.events & EPOLLOUT || fsm12.state > STATE_AUTO)
            fsm_driver(&fsm12);
        if (ev.data.fd == fd2 && ev.events & EPOLLIN || ev.data.fd == fd1 && ev.events & EPOLLOUT || fsm12.state > STATE_AUTO)
            fsm_driver(&fsm12);
    }

    fcntl(fd1, F_SETFL, fd1_save);
    fcntl(fd2, F_SETFL, fd2_save);
}

int main() {
    int fd1, fd2;
    fd1 = open(TTY1, O_RDWR);
    if (fd1 < 0) {
        perror("open()");
        exit(1);
    }
    write(fd1, "TTY1123uhiohbphpihuaspdfdsagafdghtaha7777\n", 41);
    lseek(fd1, 0, SEEK_SET);
    
    fd2 = open(TTY2, O_RDWR | O_NONBLOCK);
    if (fd2 < 0) {
        perror("open()");
        exit(1);
    }
    write(fd2, "TTY3211uhiohbphpihuaspdfdsagafdghtaha6666\n", 41);
    lseek(fd2, 0, SEEK_SET);
    relay(fd1, fd2);
    close(fd1);
    close(fd2);
    exit(0);
}