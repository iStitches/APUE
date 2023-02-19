/**
 *  池内算法：  由一个主进程分派任务，多个子进程争夺任务进行处理，任务状态由jobNum表示。主进程只有当任务被成功接收时才继续分发任务，并通知正在等待的子进程；子进程在任务
 *  处理完毕后需要通知主进程
 * */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>

#define THRNUM 50
#define LEFT 30000001
#define RIGHT 30000200

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static volatile int jobNum = 0;     //任务编号, >0 表示任务未领取、 ==0表示任务已经被领取、==-1表示所有任务处理完毕

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
        if (count == 5) {
            break;
        }
    }
    // printf("[%d] exit\n", *(int*)p);
    pthread_exit(NULL);
}

int main() 
{
    int i;
    pthread_t threads[THRNUM];

    for (i = 0; i < THRNUM; i++) {
        int *n = (int*)malloc(sizeof(int));
        *n = i;
        int err = pthread_create(&threads[i], NULL, primer_handler, n);
        if (err) {
            fprintf(stderr, "%s\n", strerror(err));
            exit(1);
        }
    }

    for (i = LEFT; i <= RIGHT; i++)
    {
        pthread_mutex_lock(&mutex);
        
        while(jobNum != 0) {
            pthread_cond_wait(&cond, &mutex);
        }
        //分发任务
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
    printf("all task over\n");
    jobNum = -1;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);

    for (i = 0; i < THRNUM; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    exit(0);
}