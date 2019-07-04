---
layout:     post
title:      C++ 并发编程笔记(一)
subtitle:   C++ 并发编程笔记(一)
date:       2019-07-03
author:     王鹏程
header-img: img/post-bg-ios10.jpg
catalog: true
tags:
    - C++
    - 基础编程
---

> 2019-7-3 20:56:48 

# C++并发编程阅读笔记

## 第1章 你好，C++的并发世界

### 1.1.1  何谓并发

**并发**：CPU交替使用时钟，模拟并发

**并行**：线程的真正并行执行

![多线程并行比较](../img/2019-07-03 21-04-58.png)

#### 1.1.2  并发的途径

- 多进程并发：将应用程序分为多个独立的进程,它们在同一时刻运行,就像同
时进行网页浏览和文字处理一样。
- 多线程并发：在单个进程中运行多个线程。线程很像轻量级的进程:每个线程相互独
立运行,且线程可以在不同的指令序列中运行。进程中的所有线程都共享地址空间,
并且所有线程访问到大部分数据———全局变量仍然是全局的,指针、对象的引用或数据可
以在线程之间传递。

**注意：Linux 一般允许的最大堆栈为8-10M，需要自己设置扩容**

程序并发的方式有2种：

- 程序执行并行：不同的线程执行，不同的过程
- 数据并行：不同的线程执行相同的程序，处理不同的数据。

### 1.4 开始入门
开始多线程编程的一个简单例子：

```c++
#include <iostream>
#include <thread>
void hello()
{
    std::cout<<"Hello Concurrent World\n";
}
int main()
{
    //设置线程初始函数名称

    std::thread t(hello); 
    t.join();
}
```

## 第2章  线程管理


### 2.1 线程管理的基础

main()函数就是一个程序的主要线程，其它线程主要由函数的入口所决定。当线程执行完入口函数后，线程也会退出。

#### 2.1.1 启动线程

_参考链接:_ [c++并发编程](https://www.cnblogs.com/zhanghu52030/p/9166526.html)

线程在`std::thread`对象构造时就开始启动了，无返回值时，启动后自动结束，存在返回值时，参数传递完成后结束。

`std::thread`允许使用带有函数调用符类型的实例传入`std::thread`类中，来替换默认的构造函数

```c++

class background_task
{
public:
    void operator()() const
    {
        do_something();
        do_something_else();
    }
};

background_task f;
std::thread my_thread(f);


```

注意： 当把函数对象传入到线程构造函数中时，如果你传递一个临时变量，而不是一个命名的变
量;C++编译器会将其解析为函数声明,而不是类型对象的定义。

```c++
std::thread my_thread(background_task());

// 声明了一个名为my_thread的函数，函数指针指向没有参数并返回background_task对象的函数；返回一个std::thread对象的函数，而非启动了一个线程

//使用多组括号或者新的统一初始化语法可以避免这个问题

std::thread my_thread((background_task())); 
std::thread my_thread{background_task()};

//使用lambda表达式也能避免这个问题

std::thread my_thread([]{
    do_something();
    do_something_else();
});

```
如果没有指定线程的销毁`std::thread`对象会在析构函数中调用`std::terminate()`进行对象销毁和析构。

注意：

- **如果需要自己指定线程销毁，必须在`std::thread`对象销毁之前决定**
- **如果不主动等待线程结束，就必须保证线程结束之前，数据的有效性，避免线程还未结束，函数已经退出，变量成为销毁的局部变量**

```c++
struct func
{
    int& i;
    func(int& i_) :i(i_){}
    void operator() ()
    {
        for (unsigned   j=0 ;   j<1000000   ;   ++j)
        {
                do_something(i);
                //  1.  潜在访问隐患:悬空引用

        }
    }
};
void oops()
{
    int some_local_state=0;
    func my_func(some_local_state);
    std::thread my_thread(my_func);
    my_thread.detach();                                    
    //  2.  不等待线程结束

}                                                                              //  3.  新线程可能还在运行


```

对于上述的处理方法是将数据复制到线程中，使得线程函数的功能齐全。而非复制到共享数据中。此外,可以通过join()函数来确保线程在函数完成前结束。但是join()只是简单粗暴的等待线程完成或者不等待。等待线程完成的例子如下：

```c++
struct  func;   //  定义在清单2.1中
void    f()
{
        int some_local_state=0;
        func my_func(some_local_state);
        std::thread t(my_func);
        //使用异常捕获，保证访问本地状态的线程退出后，函数才结束

        try
        {
                do_something_in_current_thread();
        }
        catch(...)
        {
                t.join();       //  1
                throw;
        }
        t.join();       //  2
}

//使用“资源获取即初始化方式”(RAII,Resource  Acquisition Is  Initialization)等待线程退出
class thread_guard
{
    std::thread& t;
public:
    explicit thread_guard(std::thread& t_):t(t_)
    {

    }
    ~thread_guard()
    {
        //判断线程是否已经开始加入

        if(t.joinable())
        {
            //析构函数中使得线程被加入到原始线程中

            t.join();

        }
    }
    thread_guard(thread_guard&)=delete;

    thread_guard& operator=(thread_guard const&)=delete
};
struct func;

void f()
{
    int some_local_state=0;
    func my_func(some_local_state);
    std::thread t(my_function);
    thread_guard g(t);
    thread_guard g(t);
    do_something_in_current_thread();
}
//函数结束，局部对象开始逆序销毁，

```
##### 2.1.4 后台运行线程

使用`detach()`会让线程在后台运行。意味着主线程不能与该线程产生直接互交。使得线程实现分离，并且由C++编译器实现资源的回收。

通常称分离线程为守护线程(daemon threads);UNIX中守护线程是指,没有任何显式的用户接
口,并在后台运行的线程。这种线程的特点就是长时间运行;线程的生命周期可能会从某一个应用起始到结束,可能会在后台监视文件系统,还有可能对缓存进行清理,亦或对数据结构进行优化。另一方面,分离线程的另一方面只能确定线程什么时候结束,发后即忘(fire and forget)的任务就使用到线程的这种方式。

```c++
std::thread t(do_background_work);
t.detach();
assert(!t.joinable());

//线程分离之后与之前的主线程无关

//多线程处理文档的示例：

void    edit_document(std::string   const&  filename)
{
        open_document_and_display_gui(filename);
        while(!done_editing())
        {
                user_command  cmd=get_user_input();
                if(cmd.type==open_new_document)
                {
                        std::string const   new_name=get_filename_from_user();
                        //启动一个新线程开始显示和处理文档

                        std::thread t(edit_document,new_name);      //  1
                        //分离线程

                        t.detach();     
                }
                else
                {
                            process_user_input(cmd); 
                }
        }
}

```

#### 2.2 向线程函数传递参数

_参考链接：_ [C++11的6种内存序总结](https://blog.csdn.net/lvdan1/article/details/54098559);

线程调用的默认参数要拷贝到线程独立内存中，几十参数是引用的形式，也可以在新线程中进行访问。

```c++
void f(int i, std::string const& s);
std::thread t(f,3,"hello");
```

注意： **指向动态变量的指针作为参数传递给线程的情况，可能在传递过程中产生未定义的行为。使得程序崩溃**

```c++

void f(int   i,std::string   const&  s);
void oops(int    some_param)
{
    //指针变量

    char buffer[1024];
    //指针变量指向输入参数

    sprintf(buffer, "%i",some_param);
    //创建线程，输入指针函数

    std::thread t(f,3,buffer); 
    t.detach();

}

//在从char*到std::string类型转换的过程中，函数很有可能在转化成功之前崩溃；但是`std::thread`的构造函数会复制提供的变量，就只复制了没有转换成期望类型的字符串字面值，最终造成程序崩溃。

//只要能在构造函数之前将字面值转换为`string`对象就可以了。

void f(int i,std::string const& s);

void not_oops(int some_param)
{
    char buffer[1024];
    sprintf(buffer,"%i",some_param);
    //使用显示转换避免指针悬垂

    std::thread t(f,3,std::string(buffer)); 
    t.detach();
}

```

注意： 
- 当想要在线程构造函数中传递参数为引用参数，使得变量在线程中进行更改的时候，需要使用`std::ref`将参数转换成为引用的形式，避免其在构造的过程中使用默认拷贝。如`std::thread t(update_data_for_widget,w,std::ref(data));`;这样data就在线程前后发生了改变。
- 可以使用`std::move`将一个参数，移动到线程中去。

```c++
//输如参数是一个只允许一个使用的指针 

void process_big_object(std::unique_ptr<big_object>);

std::unique_ptr<big_object> p(new big_object);
p->prepare_data(42);
//使用move函数，将指针的所有权，交给线程内部的函数库

std::thread t(process_big_object,std::move(p));

```

#### 2.3 转移线程所有权
为了保证线程控制句柄的有效性，`std::thread`支持使用`std:move()`函数实现函数的转移，**但是线程的拷贝构造和赋值操作符决定了，它在进行复制时会先使用`std::terminate()`强制结束原来已有的线程，再接受新句柄；即说明：不能通过赋一个新值给std::thread 对象的方式来“丢弃”一个线程**。

```c++
void some_function();
void some_other_function();
//直接构造线程对象

std::thread t1(some_function); 
//移动线程句柄到t2，之后t1无用

std::thread t2=std::move(t1);
//创建拷贝的时候，默认隐式调用std::move()函数

t1=std::thread(some_other_function);
//创建线程对象

std::thread t3;
//t2线程句柄，交给t3 

t3=std::move(t2);
//t1执行std::terminate()终止程序继续运行，保证与std::thread的析构函数行为一致，需要在

t1=std::move(t3);

```
建议的线程拷贝使用如下：

```c++
class   scoped_thread
{
        std::thread t;
public:
    //直接获取线程的句柄函数

    explicit  scoped_thread(std::thread   t_):
                t(std::move(t_))
    {
        //线程以及结束过一次就返回失败

        if(!t.joinable())
            throw   std::logic_error(“No    thread”);
    }
    //析构函数结束线程

    ~scoped_thread()
    {
        t.join();
    }
    //拷贝构造函数
    
    scoped_thread(scoped_thread const&)=delete;
    //赋值操作符

    scoped_thread&  operator=(scoped_thread const&)=delete;
};
struct  func;   //  定义在清单2.1中
void f()
{
    int some_local_state;
    //条用拷贝构造函数

    scoped_thread   t(std::thread(func(some_local_state)));
    //执行程序

    do_something_in_current_thread();
}                                                                                                           
void do_work(unsigned    id);

void f1()
{
    std::vector<std::thread>    threads;
    for(unsigned i=0; i<20; ++i)
        {
            //产生线程

            threads.push_back(std::thread(do_work,i));  
        }
        //对每个线程调用join()

        std::for_each(threads.begin(),threads.end(),std::mem_fn(&std::thread::join));  
}

```

### 2.4 运行时决定线程数量

`std::accumulate`可以用来计算容器中元素的和，总体思路是利用`std::thread::hardware_concurrency()`返回并发在一个程序中的线程数量，由此来决定最终的线程数目。

```c++
template<typename Iterator,typename T>
//定义小块计算的类

struct accumulate_block
{
    void operator()(Iterator frist,Iterator last,T& result)
    {
        //使用计算结果

        result=std::accumulate(first,last,result);
    }
};

//定义并行计算的模板函数

template<typename Iterator,typename T>

T parallel_accumulate(Iterator first,Iterator last,T init)
{
    //计算元素的总数量

    unsigned long const length=std::distance(first,last);
    //长度为0直接返回初始值

    if(!length)
    {
        return init;
    }
    //最小线程数量

    unsigned long const min_per_thread=25；
    //最大线程数量

    unsigned long const max_threads=(length+min_per_thread-1)/min_per_thread;
    //计算正在运行的线程数量

    unsigned long const hardware_threads=std::thread::hardware_concurrency();
    //计算真正的线程数量

    unsigned long const num_threads=std::min(hardware_threads!=0?hardware_threads:2,max_threads);
    //计算分块运行的数量

    unsigned long const block_size=length/num_threads;
    //准备结果数据

    std::vector<T> results(num_threads);
    //准备线程

    std::vector<std::thread> threads(num_threads-1);
    //遍历初始化数据
    Iterator block_start=frist;

    for(unsigned long i=0;i<(num_threads-1);++i)
    {
        //临时分块的终点

        Iterator block_end=block_start;
        //移动迭代器将其指向末尾

        std::advance(block_end,block_size);
        //初始化线程

        threads[i]=std::thread(
            accumulate_block<Iterator,T>(),
            block_start,
            block_end,
            std::ref(results[i])
            );
        //更新迭代器位置

        block_start=block_end;
    }
    //将剩余的数全部分到最后一个线程块

    accumulate_block<Iterator,T>()(
        block_start,last,results[num_threads-1]
        );
    //等待创建线程的完成
    
    std::for_each(threads.begin(),
        threads.end(),
        std::mem_fn(&std::thread::join)
        );
    //计算求和

    return std::accumulate(results.begin(),results.end(),init);
}
//注意：这里的迭代器都必须是前向迭代器，并且必须保证T存在默认的构造函数

```

### 2.5 识别线程

线程标示类型是`std::thread::id`，可以通过两种方式进行检索。

- `std::thread::get_id()`函数直接来获取。当没有绑定线程时，函数返回`std::thread::type`
- 在当前线程中调用`std::this_thread::get_id()`获取当前的线程标识符











