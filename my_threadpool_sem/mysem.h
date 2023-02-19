#ifndef MESEM_H__
#define MYSEM_H__

typedef void mysem_t;

mysem_t* mysem_init(int initival);

int mysem_add(mysem_t *sem, int);

int mysem_sub(mysem_t *sem, int);

int mysem_destory(mysem_t *sem);

#endif
