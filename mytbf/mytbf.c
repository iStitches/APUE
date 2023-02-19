#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "mytbf.h"

struct mytbf_st {
    int cps;              //流量
    int brust;        //上限
    int token;        //当前可用令牌数量
    int pos;              //当前桶的位置
    pthread_mutex_t mut;  //当前桶的锁
    pthread_cond_t  cond; //条件变量
};

//所有令牌桶
static struct mytbf_st* job[MYTBF_MAX];
//令牌桶互斥量
static pthread_mutex_t mut_job = PTHREAD_MUTEX_INITIALIZER;
//添加令牌的线程
static pthread_t tid;
//初始化添加令牌线程,仅执行一次
static pthread_once_t once = PTHREAD_ONCE_INIT;

//add_token
static void* add_token(void *p) {
    int i;
    while(1)
    {
        pthread_mutex_lock(&mut_job);
        for (i = 0; i < MYTBF_MAX; i++) {
            if (job[i] != NULL) {
                pthread_mutex_lock(&job[i]->mut);
                job[i]->token += job[i]->cps;
                if (job[i]->token > job[i]->brust)
                    job[i]->token = job[i]->brust;
                //添加令牌后应该通知条件变量
                pthread_cond_broadcast(&job[i]->cond);
                pthread_mutex_unlock(&job[i]->mut);
            }
        }
        pthread_mutex_unlock(&mut_job);
        sleep(1);
    }
    pthread_exit(NULL);
}

//module_unload
static void module_unload(void) {
    int i;
    pthread_cancel(tid);
    pthread_join(tid, NULL);

    pthread_mutex_lock(&mut_job);
    for (i = 0; i < MYTBF_MAX; i++) {
        if (job[i] != NULL) {
            pthread_mutex_destroy(&job[i]->mut);
            pthread_mutex_destroy(&job[i]->cond);
            free(job[i]);
        }
    }
    pthread_mutex_unlock(&mut_job);
    pthread_mutex_destroy(&mut_job);
}

//令牌桶模块初始化,创建自动增加令牌的线程
static void module_load(void) {
    int err = pthread_create(&tid, NULL, add_token,NULL);
    if (err) {
        fprintf(stderr, "pthread_create(): %s\n", strerror(err));
        exit(1);
    }
    //异常或者程序结束时需要还原模块状态,atexit实现
    atexit(module_unload);
}

//获取空闲的令牌桶位置
static int get_free_pos_unlocked() {
    int i;
    for (i = 0; i < MYTBF_MAX; i++) {
        if (job[i] == NULL)
            return i;
    }
    return -1;
}

//初始化令牌桶
mytbf_t *mytbf_init(int caps, int brust) {
    struct mytbf_st* me;
    int pos;
    //加载令牌桶模块
    pthread_once(&once, module_load);
    me = malloc(sizeof(*me));
    if (me == NULL) {
        return NULL;
    }
    me->cps = caps;
    me->brust = brust;
    pthread_mutex_init(&me->mut, NULL);
    pthread_cond_init(&me->cond, NULL);
    pthread_mutex_lock(&mut_job);
    pos = get_free_pos_unlocked();
    if (pos < 0) {
        pthread_mutex_unlock(&mut_job);
        free(me);
        return NULL;
    }
    me->pos = pos;
    job[pos] = me;
    pthread_mutex_unlock(&mut_job);
    return me;
}

//获取令牌
int mytbf_fetchtoken(mytbf_t* tb, int size) {
    int n;
    struct mytbf_st *me = tb;
    if (size < 0) {
        return -EINVAL;
    }

    pthread_mutex_lock(&me->mut);
    //没有令牌，先让出锁给其它线程使用
    while (me->token <= 0) {
        //这里采取忙等方式,不断释放锁等待然后获取锁判断是否有空闲token可用
        //pthread_mutex_unlock(&me->mut);
        //sched_yield();
        //pthread_mutex_lock(&me->mut);
        pthread_cond_wait(&me->cond, &me->mut);  //等待通知获取锁
    }

    n = size > me->token ? me->token : size;
    me->token -= n;
    pthread_mutex_unlock(&me->mut);
    return n;
}

//归还令牌
int mytbf_returntoken(mytbf_t* tb, int size) {
    struct mytbf_st *me = tb;
    if (size < 0) {
        return -EINVAL;
    }
    pthread_mutex_lock(&me->mut);
    me->token += size;
    if (me->token > me->brust) {
        me->token = me->brust;
    }
    pthread_cond_broadcast(&me->cond);
    pthread_mutex_unlock(&me->mut);
    return size;
}

//销毁令牌桶
void mytbf_destory(mytbf_t* tb) {
    struct mytbf_st *me = tb;
    pthread_mutex_lock(&mut_job);
    job[me->pos] = NULL;
    pthread_mutex_unlock(&mut_job);
    pthread_mutex_destroy(&me->mut);
    free(me);
}