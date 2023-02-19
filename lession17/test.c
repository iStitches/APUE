/*
这是一段伪代码,看懂意思就成
main里面不断判断线程,如果test_thd存在就删除,如果不存在,就创建.
这里面创建线程的属性统一为缺省状态(PTHREAD_CANCEL_ENABLE).
*/

static sem_t mutex;
static int iswork = 0;  //用来判断线程状态
static void *test_thread_handler();
static void *select_read();

int main( int argc, char *argv[] )
{
    pthread_t test_thd;
    iswork = 0;
    sem_init(&mutex, 0, 1 ); //初始化锁
    while(1)
    {
        get_task();  //阻塞等待任务到来（消息队列阻塞）
        if(iswork)
        {
            pthread_cancel(test_thd);
            iswork = 0;
        }
        else
        {
            sem_wait(&mutex);  //加锁  (0,0)->(0,1)->(0,0)
            write();  //写串口
            sem_post(&mutex);  //解锁  (0,0)->(0,1)

            pthread_create(&test_thd,NULL, test_thread_handler,NULL);
        }
    }

    return 0;
}

static void *test_thread_handler()
{
    iswork = 1;
    int time = 5;   //5秒
    pthread_detach(pthread_self());     //分离
    int ret = socket_select(time);//一个5秒的阻塞,时间内有触发立即返回1
    printf("select over %d\t%d\n",ret,time);
    if(ret > 0)
    {
        select_read();
    }
    printf("thread over\n");
    iswork = 0;
    return 0;
}

static void select_read()
{
    sem_wait(&mutex);  //加锁  (0,0)->(0,1)->(0,0)
    read();  //读串口，未确定是否为阻塞(取消点) 
    sem_post(&mutex);  //解锁  (0,0)->(0,1)
    printf("read over\n");
}