#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>

#define BUFLINE 1024
#define THREAD_NUM 20
#define FNAME "/tmp/out"

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static void * th_handler(void * p) {
    

    FILE* fp;
    fp = fopen(FNAME, "r+");
    if (fp == NULL) {
        perror("fopen()");
        exit(1);
    }
    char buffer[BUFLINE];
    
    pthread_mutex_lock(&mutex);
    fgets(buffer, BUFLINE, fp);
    fseek(fp, 0, SEEK_SET);
    fprintf(fp, "%d\n", atoi(buffer)+1);
    fclose(fp);
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int i;
    pthread_t p[THREAD_NUM];
    for (i = 0; i < THREAD_NUM; i++)
    {
        pthread_create(&p[i], NULL, th_handler, NULL);
    }

    for (i = 0; i < THREAD_NUM; i++) {
        pthread_join(p[i], NULL);
    }
}