---
layout:     post
title:      Linux程序设计 学习笔记 (四)
subtitle:   Linux程序设计 学习笔记 (四)
date:       2019-09-18
author:     王鹏程
header-img: img/post-bg-ios10.jpg
catalog: true
tags:
    - C/C++
    - Linux
    - 操作系统
    - 程序设计
---

> 2019-09-18: 21:29:53

# Linux 程序设计 阅读笔记(四)

## 参考链接：

- [Linux内核文档首页](https://www.kernel.org/doc/Documentation/)
- [Linux文档](https://linux.die.net/)
- [Linux c 开发手册](https://legacy.gitbook.com/book/wizardforcel/linux-c-api-ref/details)
- [Linux Kernel API](https://www.kernel.org/doc/htmldocs/kernel-api/index.html)
- [书中代码地址](http://www.wrox.com/WileyCDA/WroxTitle/Beginning-Linux-Programming-4th-Edition.productCd-0470147628,descCd-DOWNLOAD.html)
- [POSIX thread (pthread) libraries](https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html)



## 第 12 章 POSIX线程

### 12.1 什么是线程

线程：一个程序中的多个执行线路就叫线程(thread)。或者说线程是一个进程内部的一个控制序列。

当使用fork()函数时，创建一个进程，这个新进程拥有自己的变量和自己的PID.时间调度也是独立的，并且拥有自己的栈。与创建者共享全局变量、文件描述符、信号处理函数和当前目录状态

编写多线程程序时，我们通过定义宏`_REENTRANT`来告诉编译器我们需要可重入功能，这个宏的定义必须位于程序中的任何#include之前。

```c
#include <pthread.h>
//创建利用函数和相关参数创建线程

int pthread_create(pthread_t *thread,pthread_attr_t *attr,void *(*start_rountine)(void *),void *arg);
//终止相关线程

void pthread_exit(void *retval);
//收集子进程信息的wait函数
int pthread_join(pthread_t th,void **thread_return);
```

下面是一个简单的pthread使用示例：

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

void *thread_function(void *arg);

char message[] = "Hello World";

int main() {
    int res;
    pthread_t a_thread;
    void *thread_result;
    //创建对应的函数

    res = pthread_create(&a_thread, NULL, thread_function, (void *)message);
    if (res != 0) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Waiting for thread to finish...\n");
    res = pthread_join(a_thread, &thread_result);
    if (res != 0) {
        perror("Thread join failed");
        exit(EXIT_FAILURE);
    }
    printf("Thread joined, it returned %s\n", (char *)thread_result);
    printf("Message is now %s\n", message);
    exit(EXIT_SUCCESS);
}

void *thread_function(void *arg) {
    printf("thread_function is running. Argument was %s\n", (char *)arg);
    sleep(3);
    strcpy(message, "Bye!");
    pthread_exit("Thank you for the CPU time");
}
```

### 12.4 同时执行

下面来编写一个程序让两个同时进行

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

void *thread_function(void *arg);
int run_now = 1;
char message[] = "Hello World";

int main() {
    int res;
    pthread_t a_thread;
    void *thread_result;
    int print_count1 = 0;
    //创建线程

    res = pthread_create(&a_thread, NULL, thread_function, (void *)message);
    if (res != 0) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }
    //在这里等待一段时间
    while(print_count1++ < 20) {
        if (run_now == 1) {
            printf("1");
            run_now = 2;
        }
        else {
            sleep(1);//没有则不断检查，这种方式称之为忙等待
        }
    }

    printf("\nWaiting for thread to finish...\n");
    //等待线程完成和结束
    res = pthread_join(a_thread, &thread_result);
    if (res != 0) {
        perror("Thread join failed");
        exit(EXIT_FAILURE);
    }
    printf("Thread joined\n");
    exit(EXIT_SUCCESS);
}

void *thread_function(void *arg) {
    int print_count2 = 0;

    while(print_count2++ < 20) {
        if (run_now == 2) {
            printf("2");
            run_now = 1;
        }
        else {
            sleep(1);
        }
    }

    sleep(3);
}

```
### 12.5 同步

同步的两种基本方法：信号量和互斥量。

#### 12.5.1 用信号量进行同步

信号量函数的名字都以`sem_`开头，而不想大多数线程函数那样以pthread_开头。线程中使用的基本信号量函数有4个，它们都非常简单

```c
#include <semaphore.h>
//初始化sem指向的信号量对象，设置共享量和初始值

int sem_init(sem_t *sem,int pshared,unsigned int value);
//等待传入信号量，以原子操作的方式将信号量-1

int sem_wait(sem_t *sem);
//发射信号量,以原子操作的方式将信号量+1

int sem_post(sem_t *sem);
//在使用玩信号量之后，对其进行销毁

int sem_destroy(sem_t *sem);
```

信号量的简单使用
```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

void *thread_function(void *arg);
sem_t bin_sem;

#define WORK_SIZE 1024
char work_area[WORK_SIZE];

int main() {
    int res;
    pthread_t a_thread;
    void *thread_result;

    res = sem_init(&bin_sem, 0, 0);
    if (res != 0) {
        perror("Semaphore initialization failed");
        exit(EXIT_FAILURE);
    }
    res = pthread_create(&a_thread, NULL, thread_function, NULL);
    if (res != 0) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }

    printf("Input some text. Enter 'end' to finish\n");
    while(strncmp("end", work_area, 3) != 0) {
      if (strncmp(work_area, "FAST", 4) == 0) {
        sem_post(&bin_sem);
        strcpy(work_area, "Wheeee...");
      } else {
        fgets(work_area, WORK_SIZE, stdin);
      }
      sem_post(&bin_sem);
    }

    printf("\nWaiting for thread to finish...\n");
    res = pthread_join(a_thread, &thread_result);
    if (res != 0) {
        perror("Thread join failed");
        exit(EXIT_FAILURE);
    }
    printf("Thread joined\n");
    sem_destroy(&bin_sem);
    exit(EXIT_SUCCESS);
}

void *thread_function(void *arg) {
    sem_wait(&bin_sem);
    while(strncmp("end", work_area, 3) != 0) {
        printf("You input %d characters\n", strlen(work_area) -1);
        sem_wait(&bin_sem);
    }
    pthread_exit(NULL);
}

```

#### 12.5.2 用互斥量进行同步

它允许程序员锁住某个对象，使得每次只有一个线程访问它。进入这段代码之前锁住一个互斥量，在完成操作之后解锁它

```c
#include <pthread.h>
//初始化互斥量

int pthread_mutex_init(pthread_mutex_t *mutex,const pthread_mutexattr_t);
//互斥量加锁

int pthread_mutex_lock(pthread_mutex_t *mutex);
//互斥量解锁

int pthread_mutex_unlock(pthread_mutex_t *mutex);
//互斥量销毁

int pthread_mutex_destroy(pthread_mutex_t *mutex);

```
注意：互斥量的属性相关值默认为fast，如果一个程序试图对一个已经加了锁的互斥量调用thread_mutex_lock程序就会被阻塞，当互斥量拥有线程被阻塞时，就会形成死锁。

下面是使用的简单示例：

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

void *thread_function(void *arg);
pthread_mutex_t work_mutex; /* protects both work_area and time_to_exit */

#define WORK_SIZE 1024
char work_area[WORK_SIZE];
int time_to_exit = 0;

int main() {
    int res;
    pthread_t a_thread;
    void *thread_result;
    //初始化信号量

    res = pthread_mutex_init(&work_mutex, NULL);
    if (res != 0) {
        perror("Mutex initialization failed");
        exit(EXIT_FAILURE);
    }
    //创建线程

    res = pthread_create(&a_thread, NULL, thread_function, NULL);
    if (res != 0) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }
    //线程加锁

    pthread_mutex_lock(&work_mutex);
    //执行操作

    printf("Input some text. Enter 'end' to finish\n");
    //接受输入参数

    while(!time_to_exit) {
        fgets(work_area, WORK_SIZE, stdin);
        pthread_mutex_unlock(&work_mutex);
        //循环等待并加锁

        while(1) {
            pthread_mutex_lock(&work_mutex);
            if (work_area[0] != '\0') {//读到末尾直接解锁
                pthread_mutex_unlock(&work_mutex);
                sleep(1);
            }
            else {
                break;
            }
        }
    }
    //解锁线程
    pthread_mutex_unlock(&work_mutex);
    printf("\nWaiting for thread to finish...\n");
    //等待子线程结束
    res = pthread_join(a_thread, &thread_result);
    if (res != 0) {
        perror("Thread join failed");
        exit(EXIT_FAILURE);
    }
    printf("Thread joined\n");
    //销毁信号量

    pthread_mutex_destroy(&work_mutex);
    exit(EXIT_SUCCESS);
}

void *thread_function(void *arg) {
    sleep(1);
    //互斥量加锁

    pthread_mutex_lock(&work_mutex);
    //当检测到了end函数

    while(strncmp("end", work_area, 3) != 0) {
        printf("You input %d characters\n", strlen(work_area) -1);
        work_area[0] = '\0';
        pthread_mutex_unlock(&work_mutex);
        sleep(1);
        pthread_mutex_lock(&work_mutex);
        while (work_area[0] == '\0' ) {
            pthread_mutex_unlock(&work_mutex);
            sleep(1);
            pthread_mutex_lock(&work_mutex);
        }
    }
    time_to_exit = 1;
    work_area[0] = '\0';
    pthread_mutex_unlock(&work_mutex);
    pthread_exit(0);
}

```
实际工作中我们应该尽量使用信号量来避免出现轮寻的情况。

### 12.6 线程的属性

_参考链接：_ [POSIX thread (pthread) libraries](https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html)


在上一节中我们了解到，线程的属性的存在,我们可以使用。对应的函数对相关的函数进行修改。

```c
#include <pthread.h>
//初始化属性

int pthread_attr_init(pthread_attr_t *attr);
//设置detachedstate属性，是否可以获取另外一个线程的状态。
int pthread_attr_setdetachstate(pthread_attr_t *attr,int detachstate);
int pthread_attr_getdetachstate(const pthread_attr_t *attr,int *detachstate);
//设置schedpolicy线程的调度方式
int pthread_attr_setschedpolicy(pthread_attr_t *attr,int policy);
int pthread_attr_getschedpolicy(const pthread_attr_t *attr,int *policy);
//和上一个属性结合使用。可以对SCHED_OTHER策略运行的线程的调度进行控制
int pthread_attr_setschedparam(pthread_attr_t *attr,const struct sched_param *param);
int pthread_attr_getschedparam(const pthread_attr_t *attr,const struct sched_param *param);
//是否继承其创建者使用的参数。
int pthread_attr_setinheritsched(pthread_attr_t *attr, int inherit);
int pthread_attr_getinheritsched(const pthread_attr_t *attr, int *inherit);
//控制线程调度的计算方式
int pthread_attr_setscope(pthread_attr_t *attr,int scope);
int pthread_attr_getscope(const pthread_attr_t *attr,int *scope);
//控制线程创建栈的大小
int pthread_attr_setstacksize(pthread_attr_t *attr,int scope);
int pthread_attr_getstacksize(const pthread_attr_t *attr,int *scope);
```

下面是一个简单的使用示例

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

void *thread_function(void *arg);
pthread_mutex_t work_mutex; /* protects both work_area and time_to_exit */

#define WORK_SIZE 1024
char work_area[WORK_SIZE];
int time_to_exit = 0;

int main() {
    int res;
    pthread_t a_thread;
    void *thread_result;
    res = pthread_mutex_init(&work_mutex, NULL);
    if (res != 0) {
        perror("Mutex initialization failed");
        exit(EXIT_FAILURE);
    }
    res = pthread_create(&a_thread, NULL, thread_function, NULL);
    if (res != 0) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_lock(&work_mutex);
    printf("Input some text. Enter 'end' to finish\n");
    while(!time_to_exit) {
        fgets(work_area, WORK_SIZE, stdin);
        pthread_mutex_unlock(&work_mutex);
        while(1) {
            pthread_mutex_lock(&work_mutex);
            if (work_area[0] != '\0') {
                pthread_mutex_unlock(&work_mutex);
                sleep(1);
            }
            else {
                break;
            }
        }
    }
    pthread_mutex_unlock(&work_mutex);
    printf("\nWaiting for thread to finish...\n");
    res = pthread_join(a_thread, &thread_result);
    if (res != 0) {
        perror("Thread join failed");
        exit(EXIT_FAILURE);
    }
    printf("Thread joined\n");
    pthread_mutex_destroy(&work_mutex);
    exit(EXIT_SUCCESS);
}

void *thread_function(void *arg) {
    sleep(1);
    pthread_mutex_lock(&work_mutex);
    while(strncmp("end", work_area, 3) != 0) {
        printf("You input %d characters\n", strlen(work_area) -1);
        work_area[0] = '\0';
        pthread_mutex_unlock(&work_mutex);
        sleep(1);
        pthread_mutex_lock(&work_mutex);
        while (work_area[0] == '\0' ) {
            pthread_mutex_unlock(&work_mutex);
            sleep(1);
            pthread_mutex_lock(&work_mutex);
        }
    }
    time_to_exit = 1;
    work_area[0] = '\0';
    pthread_mutex_unlock(&work_mutex);
    pthread_exit(0);
}
```

可以使用`sched_get_priority_max`和`sched_get_priority_min`这两个函数来查找可用的优先级级别。

使用示例如下：

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

void *thread_function(void *arg);

char message[] = "Hello World";
int thread_finished = 0;

int main() {
    int res;
    pthread_t a_thread;
    void *thread_result;
    pthread_attr_t thread_attr;
    //定义一些额外的变量

    int max_priority;
    int min_priority;
    struct sched_param scheduling_value;
    //设置对应的调度策略

    res = pthread_attr_init(&thread_attr);
    if (res != 0) {
        perror("Attribute creation failed");
        exit(EXIT_FAILURE);
    }
    res = pthread_attr_setschedpolicy(&thread_attr, SCHED_OTHER);
    if (res != 0) {
        perror("Setting schedpolicy failed");
        exit(EXIT_FAILURE);
    }
    res = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
    if (res != 0) {
        perror("Setting detached attribute failed");
        exit(EXIT_FAILURE);
    }
    res = pthread_create(&a_thread, &thread_attr, thread_function, (void *)message);
    if (res != 0) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }
    //查找允许的优先级范围

    max_priority = sched_get_priority_max(SCHED_OTHER);
    min_priority = sched_get_priority_min(SCHED_OTHER);
    //设置优先级

    scheduling_value.sched_priority = min_priority;
    res = pthread_attr_setschedparam(&thread_attr, &scheduling_value);
    if (res != 0) {
        perror("Setting schedpolicy failed");
        exit(EXIT_FAILURE);
    }
    (void)pthread_attr_destroy(&thread_attr);
    while(!thread_finished) {
        printf("Waiting for thread to say it's finished...\n");
        sleep(1);
    }
    printf("Other thread finished, bye!\n");
    exit(EXIT_SUCCESS);
}

void *thread_function(void *arg) {
    printf("thread_function is running. Argument was %s\n", (char *)arg);
    sleep(4);
    printf("Second thread setting finished flag, and exiting now\n");
    thread_finished = 1;
    pthread_exit(NULL);
}
```

### 12.7 取消一个线程

```c
#include <pthread.h>
//向线程发送取消信号

int pthread_cancel(pthread_t thread);
//设置线程自己的取消状态,state是接受/忽略取消请求，oldstate指针用于获取先前的取消状态

int pthread_setcancelstate(int state,int *oldstate);
//进入第二个层次设置取消类型

int pthread_setcanceltype(int type,int *oldtype);
```

下面是一个简答的使用示例;
```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

void *thread_function(void *arg);

int main() {
    int res;
    pthread_t a_thread;
    void *thread_result;

    res = pthread_create(&a_thread, NULL, thread_function, NULL);
    if (res != 0) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }
    sleep(3);
    printf("Canceling thread...\n");
    res = pthread_cancel(a_thread);
    if (res != 0) {
        perror("Thread cancelation failed");
        exit(EXIT_FAILURE);
    }
    printf("Waiting for thread to finish...\n");
    res = pthread_join(a_thread, &thread_result);
    if (res != 0) {
        perror("Thread join failed");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

void *thread_function(void *arg) {
    int i, res, j;
    res = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    if (res != 0) {
        perror("Thread pthread_setcancelstate failed");
        exit(EXIT_FAILURE);
    }
    res = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    if (res != 0) {
        perror("Thread pthread_setcanceltype failed");
        exit(EXIT_FAILURE);
    }
    printf("thread_function is running\n");
    for(i = 0; i < 10; i++) {
        printf("Thread is still running (%d)...\n", i);
        sleep(1);
    }
    pthread_exit(0);
}
```
### 12.8 多线程运行实例

```c
include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 6

void *thread_function(void *arg);

int main() {
    int res;
    pthread_t a_thread[NUM_THREADS];
    void *thread_result;
    int lots_of_threads;

    for(lots_of_threads = 0; lots_of_threads < NUM_THREADS; lots_of_threads++) {

        res = pthread_create(&(a_thread[lots_of_threads]), NULL, thread_function, (void *)&lots_of_threads);
        if (res != 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
        sleep(1);
    }
    printf("Waiting for threads to finish...\n");
    for(lots_of_threads = NUM_THREADS - 1; lots_of_threads >= 0; lots_of_threads--) {
        res = pthread_join(a_thread[lots_of_threads], &thread_result);
        if (res == 0) {
            printf("Picked up a thread\n");
        }
        else {
            perror("pthread_join failed");
        }
    }
    printf("All done\n");
    exit(EXIT_SUCCESS);
}

void *thread_function(void *arg) {
    int my_number = *(int *)arg;
    int rand_num;

    printf("thread_function is running. Argument was %d\n", my_number);
    rand_num=1+(int)(9.0*rand()/(RAND_MAX+1.0));
    sleep(rand_num);
    printf("Bye from %d\n", my_number);
    pthread_exit(NULL);
}
```

### 第 13 章 进程间通信：管道

管道通常是把一个进程的输出通过管道连接到另外一个进程的输入。如下所示

`cmd1 | cmd2`
shell负责安排两个命令的标准输入和标准输出

- cmd1的标准输入来自终端键盘
- cmd2的标准输出传递给cmd2,作为它的标准输入
- cmd2的标准输出链接到终端屏幕。