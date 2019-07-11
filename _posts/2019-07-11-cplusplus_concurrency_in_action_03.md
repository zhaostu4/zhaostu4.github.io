---
layout:     post
title:      C++ 并发编程笔记(三)
subtitle:   C++ 并发编程笔记(三)
date:       2019-07-06
author:     王鹏程
header-img: img/post-bg-ios10.jpg
catalog: true
tags:
    - C++
    - 基础编程
    - 多线程编程
---

>2019-07-11 20:09:48

## 第6章  基于锁的并发数据结构设计

### 6.1 并发设计的意义

同过合理设计胡吃两，让多个线程可以并发的访问这个数据，线程可以对这个数据结构做相同或者不同的操作。

**序列化(serialzation)**:线程轮流访问被保护的数据。这其实是对数据进行串行的访问,而非并发。

一般进行并发数据结构设计的思路都是：减少保护区域,减少序列化操作,就能提升并发访问的潜力。

#### 6.1.1  数据结构并发设计的指导与建议(指南)

数据结构线程安全条件:

- 确保无线程能够看到,数据结构的“不变量”破坏时的状态。
- 小心那些会引起条件竞争的接口,提供完整操作的函数,而非操作步骤。
- 注意数据结构的行为是否会产生异常,从而确保“不变量”的状态稳定。
- 将死锁的概率降到最低。使用数据结构时,需要限制锁的范围,且避免嵌套锁的存在。

需要考虑的问题：

- 锁的范围中的操作,是否允许在锁外执行?
- 数据结构中不同的区域是否能被不同的互斥量所保护?
- 所有操作都需要同级互斥量保护吗?
- 能否对数据结构进行简单的修改,以增加并发访问的概率,且不影响操作语义?

### 6.2 基于锁的并发数据结构

基于锁的并发数据结构设计,核心在于 **保证程序安全的情况下，保证线程持有锁的时间最短**。

线程安全的`stack`和`queue`示例，第三、四章中，在此不做过多叙述

#### 6.2.3  线程安全队列——使用细粒度锁和条件变量

首先先看一个单线程的队列：

```c++

template<typename T>
class queue
{
private:
    struct node
    {
        T data;
        std::unique_ptr<node> next;

        node(T data_);
        data_(std::move(data_))
        {}
    };
    //头部指针

    std::unique_ptr<node> head;
    //队尾部指针

    node* tail;
public:
    queue(){}
    queue(const queue& other)=delete;
    queue& operator=(const queue& other)=delete;
    std::shared_ptr<T> try_pop()
    {
        if(!head){
            return std::shared_ptr<T>();
        }
        std::shared_ptr<T> const res(std::make_shared<T>(std::move(head->data)));
        std::unique_ptr<node> const old_head=std::move(head);
        //将head指针指向下一个

        head=std::move(old_head->next);
        return res;
    }
//尾部插入

    void push(T new_value)
    {
        std::unique_ptr<node> p(new node(std::move(new_value)));
        node* const new_tail=p.get();

        if(tail)
        {
            tail->next=std::move(p);
        }else{
            head=std::move(p);
        }
        tail=new_tail;
    }
}

```
这里可以看到，单线程情况下，基本使用良好，但是对于多线程而言，在push和pop中没有对头尾指针添加保护锁，同时，为了防止在队列只有一个元素时的时候，head==tail；所以push和try_pop间接访问了这个头尾指针，因此需要对tail添加保护锁。
不过这里使用更简便的方法，减少锁的使用：预分配一个空节点，永远指向队列尾部，这样避免了头尾指针能够被间接访问。但是使用了一个间接层次的指针数据作为虚拟节点。

更改完成时候，在push操作中只用考虑尾部指针tail在pop函数中虽然可以访问tail但是tail只在最初阶段进行比较，更多需要考虑head。同时，添加虚拟节点意味着pop和push不能同时对同一个节点进行操作。

最终，除了操作的元素外需要上锁外，push只对tail上锁，try_pop,先对head上锁，一旦被改变之后就不再加锁。

最终结果：

线程安全队列--细粒度锁版

```c++

template <typename T>
class threadsafe_queue
{
private:
    struct node
    {
        std::shared_ptr<T> data;
        std::shared_ptr<node> next;
        
    };
    //头部节点

    std::unique_ptr<node> head;
    //尾部节点
    node* tail;
    //头部互斥保护

    std::mutex head_mutex;
    //尾部信号量

    std::mutex tail_mutex;
public:
    threadsafe_queue():head(new node),tail(head.get){}
    ~threadsafe_queue();
    threadsafe_queue(const threadsafe_queue& other)=delete;
    threadsafe_queue& operator=(const threadsafe_queue& other)=delete;


    node* get_tail()
    {
        std::lock_guard<std::mutex> tail_lock(tail_mutex);
        return tail;
    } 
    std::unique_ptr<node> pop_head()
    {
        std::lock_guard<std::mutex> head_lock(head_mutex);
        //使用git_tail 保护尾部指针一次

        if(head.get()==get_tail()){
            return nullptr;
        }
        std::unique_ptr<node> old_head=std::move(head);
        head=std::move(old_head->next);
        return old_head;
    }
    void push(T new_value)
    {
        std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value)));
        std::unique_ptr<node> p(new node);
        const* const new_tail=p.get();
        //tail 加锁

        std::lock_guard<std::mutex> tail_lock(tail_mutex);
        tail->data=new_value;
        tail->next=std::move(p);
        tail=new_tail; 
    }
    std::shared_ptr<T> try_pop()
    {
        std::unique_ptr<node> old_head=pop_head();
        return old_head?old_head->data:std::shared_ptr<T>();
    }

};

```

在此基础之上添加，可上锁和等待的线程安全队列；但是由于wait_and_pop等操作会降低程序的性能。

```c++
template<typename T>
class threadsafe_queue
{
private:
    struct  node
    {
        std::shared_ptr<T> data;
        std::unique_ptr<node> next;
    };
    std::mutex head_mutex;
    std::unique_ptr<node> head;
    std::mutex  tail_mutex;
    node* tail;
    //环境信号变量

    std::condition_variable data_cond;
public:
    threadsafe_queue():head(new node),tail(head.get()){}
    threadsafe_queue(const threadsafe_queue& other)=delete;
    threadsafe_queue& operator=(const threadsafe_queue& other)=delete;
    std::shared_ptr<T>  try_pop();
    bool try_pop(T& value);
    std::shared_ptr<T> wait_and_pop();
    void wait_and_pop(T& value);
    void push(T  new_value);
    bool empty();
};

//推入新节点

template<T>
void threadsafe_queue<T>::push(T new_data)
{
    std::shared_ptr new_data(std::make_shared<T>(std::move(new_value)));
    std::unique_ptr<node> p(new node);
    {
        //尾部加锁

        std::lock_guard<std::mutex> tail_lock(tail_mutex);
        tail->data=new_data;
        node* new_tail=p.get();
        tail->next=std::move(p);
        tail=new_tail;
    }
    //发射环境信号

    data_cond.notify_one();
}

```