#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "mysem.h"

//自定义信号量
struct mysem_st {
    int value;              //资源上限
    pthread_mutex_t mutex;  //互斥量
    pthread_cond_t cond;    //条件变量
};

//初始化信号量
mysem_t* mysem_init(int initival) {
    struct mysem_st *me;
    me = malloc(sizeof(*me));
    if (me == NULL) {
        return NULL;
    }
    me->value = initival;
    pthread_mutex_init(&me->mutex, NULL);
    pthread_cond_init(&me->cond, NULL);
    return me;
}

//归还指定数量信号量
int mysem_add(mysem_t *sem, int num) {
    struct mysem_st *me = sem;
    pthread_mutex_lock(&me->mutex);
    me->value += num;
    pthread_cond_broadcast(&me->cond);
    pthread_mutex_unlock(&me->mutex);
    return num;
}

//获取指定数量信号量
int mysem_sub(mysem_t *sem, int num) {
    struct mysem_st *me = sem;
    pthread_mutex_lock(&me->mutex);
    while (me->value < num) {
        pthread_cond_wait(&me->cond, &me->mutex);
    }
    me->value -= num;
    pthread_mutex_unlock(&me->mutex);
    return num;
}

//销毁信号量
int mysem_destory(mysem_t *sem) {
    struct mysem_st *me = sem;
    pthread_mutex_destroy(&me->mutex);
    pthread_cond_destroy(&me->cond);
    free(me);
    return 0;
}