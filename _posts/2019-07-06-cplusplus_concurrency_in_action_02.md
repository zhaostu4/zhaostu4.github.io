---
layout:     post
title:      C++ 并发编程笔记(二)
subtitle:   C++ 并发编程笔记(二)
date:       2019-07-06
author:     王鹏程
header-img: img/post-bg-ios10.jpg
catalog: true
tags:
    - C++
    - 基础编程
    - 多线程编程
---

> 2019-7-06 17:11:52

### 第4章 同步并发操作

### 4.1 等待一个事件或其他条件

在多线程同步过程中，很多时候需要进行同步等待，可以使用`std::this_thread::sleep_for()`方法来进行周期性的间歇。如下：

```c++

bool flag;
std::mutex m;

void wait_for_flag()
{
    std::unique_lock<std::mutex> lk(m);
    while(!flag) {
        //解锁互斥量

        lk.unlock();
        //休眠 100ms

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        //解锁互斥量

        lk.lock();
    }
}

```

标准库使用相关条件达成来，进行线程同步。

|方法|区别|
|:---|:---|
|`std::condition_variable`|仅限于与`std::mutex`一起工作|
|`std::condition_variable_any`|任何条件上工作，但是会产生额外的开销|

使用示例：

```c++
std::mutex mut;
//数据队列

std::queue<data_chunk> data_queue;
//环境检测变量

std::condition_variable data_cond;

void data_preparation_thread()
{
    while(more_data_to_prepare())
    {
        //准备数据

        data_chunk const  data=prepare_data();
        //当前线程加锁操作

        std::lock_guard<std::mutex> lk(mut);
        //发生环境改变信号

        data_cond.ontify_one();
    }
}

void data_processing_thread()
{
    while(true) {
        //信号量加锁

        std::unique_lock<std::mutex> lk(mut);
        //等待，直到获得响应

        data_cond.wait(
            lk,[]{return !data_queue.empty();}
            );
        //获取头部数据

        data_chunk data=data_queue.front();
        //弹出

        data_queue.pop();
        //解锁

        lk.lock();

    }
}
```

#### 4.1.2 使用条件变量构建线程安全队列

线程安全的基本操作`push()`和`wait_and_pop()`：

```c++

#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>

template <typename T>
class threadsafe_queue
{
private:
    //互斥量必须是可变的

    mutable std::mutex mut;
    std::queue<T> data_queue;
    std::condition_variable data_cond;

public:
    threadsafe_queue(){}
    threadsafe_queue(threadsafe_queue const& other)
    {
        //为其它类的信号量，加锁

        std::lock_guard<std::mutex> lk(other.mut);
        //进行拷贝

        data_queue=other.data_queue;
    }
    ~threadsafe_queue();
    void push(T new_value)
    {
        //添加锁

        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(new_value);
        //发射改变环境变量,触发一个正在等待的线程，取检查wait函数的返回状态

        data_cond.notify_one();
    }
    
    void wait_and_pop(T& value)
    {
        //添加锁

        std::unique_lock<std::mutex> lk(mut);
        //等待队列中存在数据

        data_cond.wait(lk,[this]{return !data_queue.empty();});
        //获取数据

        value=data_queue.front();
        //取出数据

        data_queue.pop();
    }
    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_lock<std::mutex> lk(mut);
        //等待直到数据为空

        data_cond.wait(lk,[this]{return !data_queue.empty();});
        //返回队列中的引用指针

        std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
        data_queue.pop();
        return res;

    }
    //非线程安全的存取函数

    bool try_pop(T& value)
    {
        std::lock_guard<std::mutex> lk(mut);
        if(data_queue.empty())
            return  false;
        value=data_queue.front();
        data_queue.pop();
        return  true;
    }
    //元素出栈

    std::shared_ptr<T> try_pop()
    {
        std::lock_guard<std::mutex> lk(mut);
        if(data_queue.empty())
        {
            return std::shared_ptr<T>();
        }
        std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
        data_queue.pop();
        return res;
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lk(mut);
        return data_queue.empty();
    }


};

```

### 4.2 使用期望等待一次性时间

C++ 中的`future`用来表示等待中的一次性事件。