#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 20
#define DATA_SIZE 1000

struct prodcons{
    int buffer[BUFFER_SIZE];
    pthread_mutex_t lock;
    pthread_cond_t notempty;
    pthread_cond_t notefull;
    int writepos, readpos;
};

struct prodcons buffer;

void init(struct prodcons *p) {
    pthread_mutex_init(&(p->lock), NULL);
    pthread_cond_init(&(p->notempty), NULL);
    pthread_cond_init(&(p->notefull), NULL);
    p->writepos = p->readpos = 0;
}

void put(struct prodcons *p, int data) {
    pthread_mutex_lock(&(p->lock));
    while((p->writepos + 1) % BUFFER_SIZE == p->readpos) {
        pthread_cond_wait(&(p->notefull), &(p->lock));
    }
    p->buffer[p->writepos] = data;
    p->writepos++;
    if (p->writepos >= BUFFER_SIZE) {
        p->writepos = 0;
    }
    pthread_cond_signal(&(p->notempty));
    pthread_mutex_unlock(&(p->lock));
}

int get(struct prodcons *p) {
    int data;
    pthread_mutex_lock(&(p->lock));
    while(p->writepos == p->readpos) {
        pthread_cond_wait(&(p->notempty), &(p->lock));
    }
    data = p->buffer[p->readpos];
    p->readpos++;
    if (p->readpos >= BUFFER_SIZE) {
        p->readpos = 0;
    }
    pthread_cond_signal(&(p->notefull));
    pthread_mutex_unlock(&(p->lock));
    return data;
}

void printData(struct prodcons *p) {
    int i;
    for (i = 0; i < p->writepos; i++) {
        printf("%d  ", p->buffer[i]);
    }
}

void *Prod(void *num) {
    int i;
    for (i = 0; i < 5; i++) {
        put(&buffer, i);
    }
    printf("producer exited\n");
    // printData(&buffer);
    pthread_exit(NULL);
}

void *Cons(void *num) {
    while (1) {
        printf("%d\n", get(&buffer));
        sleep(1);
    }
    printf("consumer exited\n");
    pthread_exit(NULL);
}

int main() {
    pthread_t prod, cons;
    int arr[3] = {1, 2, 3};
    int num = 2;
    init(&buffer);
    pthread_create(&prod, NULL, Prod, NULL);
    pthread_create(&cons, NULL, Cons, NULL);
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);
    return 0;
}