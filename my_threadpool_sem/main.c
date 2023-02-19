/**
 *  池内算法+信号量约束资源上限
 * */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include "mysem.h"

#define THRNUM 200           //总线程数
#define N 5                  //每次最多运行5个线程,限定资源上限
#define LEFT 30000001
#define RIGHT 30000200

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static volatile int jobNum = 0;     //任务编号, >0 表示任务未领取、 ==0表示任务已经被领取、==-1表示所有任务处理完毕
static mysem_t *sem;         //自定义信号量

static void* primer_handler(void *p)
{   
    int task, mark, count, j;
    count = 0;
    while (1) {
        pthread_mutex_lock(&mutex);
        while (jobNum == 0) {
            pthread_cond_wait(&cond, &mutex);
        }
        if (jobNum == -1) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        //领取任务
        task = jobNum;
        jobNum = 0;
        pthread_cond_broadcast(&cond);  //通知任务分派线程任务已被接收
        pthread_mutex_unlock(&mutex);

        //开始执行任务:判断该数字是不是素数
        mark = 1;
        for (j = 2;j <= task/2;j++){
            if (task%j == 0){
                mark = 0;
                break;
            }
        }
        if (mark) {
            printf("[%d] %d is a primer\n",*(int *)p,task);
        }
        count++;
        if (count == 1) {
            sleep(1);
            break;
        }
    }
    
    //归还计算资源
    mysem_add(sem, 1);
    pthread_exit(NULL);
}

int main() 
{
    int i;
    pthread_t threads[THRNUM];

    //初始化计算资源
    sem = mysem_init(N);


    for (i = LEFT; i <= RIGHT; i++)
    {
        mysem_sub(sem, 1);  //消耗一个计算资源
        int *ptid = malloc(sizeof(int));
        *ptid = i - LEFT;
        int err = pthread_create(&threads[i-LEFT], NULL, primer_handler, ptid);
        if (err) {
            fprintf(stderr, "%s\n", strerror(err));
            exit(1);
        }

        //分发任务
        pthread_mutex_lock(&mutex);
        while(jobNum != 0) {
            pthread_cond_wait(&cond, &mutex);
        }
        jobNum = i;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }

    //检查最后一个任务是否被接收
    pthread_mutex_lock(&mutex);
    while (jobNum != 0) {
        pthread_cond_wait(&cond, &mutex);
    }
    //任务全部下发
    // printf("all task over\n");
    jobNum = -1;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);

    for (i = LEFT; i <= RIGHT; i++) {
        pthread_join(threads[i-LEFT], NULL);
    }
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    //销毁计算资源
    mysem_destory(sem);
    exit(0);
}