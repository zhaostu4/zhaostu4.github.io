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
    node* get_tail()
    {
        std::lock_guard<std::mutex> tail_lock(tail_mutex);
        return tail;
    }
    std::unique_ptr<node> pop_head()
    {
        std::unique_ptr<node> old_head=std::move(head);
        head=std::move(old_head->next);
        return old_head;
    }
    //数据等待线程锁

    std::unique_ptr<std::mutex> wait_for_data()
    {
        std::unique_lock<std::mutex> head_lock(head_mutex);
        //等待环境唤醒

        data_cond.wait(head_lock,[&]{return head.get()!=get_tail();});
        //将锁的实例，返回给调用者

        return std::move(head_lock);
    }
    std::unique_ptr<node> wait_pop_head()
    {
        //添加数据等待线程锁

        std::unique_lock<std::mutex> head_lock(wait_for_data());

        return pop_head();
    }
    std::unique_ptr<node> wait_pop_head(T& value)
    {
        std::unique_lock<std::mutex> head_lock(wait_for_data());
        //获取头部数据

        value=std::move(*head->data);

        return pop_head();
    }

    void wait_and_pop(T& value)
    {
        std::unique_ptr<node> const old_head=wait_pop_head(value);
    }
    //试着拿出头部

    std::unique_ptr<node> try_pop_head()
    {
        std::lock_guard<std::mutex> head_lock(head_mutex);
        if(head.get()==get_tail())
        {
            return std::unique_ptr<node>();
        }
        return pop_head();
    }
    std::unique_ptr<node> try_pop_head(T& value)
    {
        std::lock_guard<std::mutex> head_lock(head_mutex);
        if(head.get()==get_tail())
        {
            return std::unique_ptr<node>();
        }
        value=std::move(*head->data);
        return pop_head();
    }

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

//线程安全队列

template<T>
std::shared_ptr<T> threadsafe_queue<T>::wait_and_pop()
{
    std::unique_ptr<node> const old_head=wait_pop_head();
    return old_head->data;
}
template<T>
void threadsafe_queue<T>::wait_and_pop(T& value)
{
    //传递值，然后返回取出的头部

    std::unique_ptr<node> const old_head=wait_pop_head(value);
}
template<T>
std::shared_ptr threadsafe_queue<T>::try_pop()
{
    std::unique_ptr<node> old_head=try_pop_head();
    return old_head?old_head->data:std::shared_ptr<T>();
}

template<T>
bool threadsafe_queue<T>::try_pop(T& value)
{
    std::unique_ptr<node> old_head=try_pop_head(value);
    return old_head;
}

template<T>
bool empty()
{
    std::lock_guard<std::mutex> head_lock(head_mutex);
    return (head.get()==get_tail());
}
```

### 6.3 基于锁设计更加复杂的数据结构

这里主要以定义一个简单的线程安全查询表和链表为例，进行工作

#### 6.3.1 一个线程安全的查询表

首先明确查询表的基本操作有：

- 添加一队“键值-数据”
- 修改指定键值所对应的数据
- 删除一组值
- 通过给定键值，获取对应数据

`std::map`椎间盘美好常见的关联容器和比较

- 二叉树；比如：红黑树：并不会提高对高并发的访问，每一个都要访问根节点，根节点需要时常上锁
- 有序数组：是最坏的选择，无法提前感知那个有序
- 哈希表：结合桶，对每个桶进行互斥加锁，提高并发性能。

```c++
//定义模板：关键字、值、hash映射

template<typename Key,typename Value,typename Hash=std::hash<Key> >



class threadsafe_lookup_table
{
private:
    //定义桶的基本类型

    class bucket_type
    {
    private:
        //设置键值对基本类型

        typedef std::pair<Key,Value>    bucket_value;
        //设置键值队列表

        typedef std::list<bucket_value> bucket_data;
        //定义列表迭代器

        typedef typename bucket_data::iterator bucket_iterator;
        //定义桶中的数据列表

        bucket_data data;
        //桶的互斥信号变量

        mutable boost::shared_mutex mutex;
        //通过关键字查找迭代器

        bucket_iterator find_entry_for(Key const& key) const
        {
            return  std::find_if(data.begin(),
                data.end(),
                [&](bucket_value const& item){return item.first==key;}
                );
        }
    public:
        //通过引入的方式，查找数据

        Value value_for(Key const& key,Value const& default_value) const
        {
            boost::shared_lock<boost::shared_mutex> lock(mutex);
            bucket_iterator const found_entry=find_entry_for(key);
            //返回查找的关键值

            return (found_entry==data.end())?default_value:found_entry->second;
        }
        //更新键值对

        void add_or_update_mapping(Key const& key,Value const& value)
        {
            std::unique_lock<boost::shared_mutex> lock(mutex);
            bucket_iterator const found_entry=find_entry_for(key);
            if(found_entry==data.end())
            {
                data.push_back(bucket_value(key,value));
            }else{
                found_entry->second=value;
            }
        }
        //移除关键字

        void remove_mapping(Key const& key)
        {
            std::unique_lock<boost::shared_mutex> lock(mutex);
            bucket_iterator const found_entry=find_entry_for(key);
            if(found_entry!=data.end())
            {
               data.erase(found_entry);
            }
        }
    };
    //end define bucket_type

    //定义查询的基本桶向量容器

    std::vector<std::unique_ptr<bucket_type> > buckets;
    //hash映射表

    Hash hasher;
    //根据关键字查找桶

    bucket_type& get_bucket(Key const& key) const
    {
        std::size_t const bucket_index=hasher(key)%buckets.size();
        return *buckets[bucket_index];
    }
//公共的类接口

public:
    //定义关键字类型

    typedef Key key_type;
    //定义映射的值

    typedef Value mapped_type;
    //定义hash函数

    typedef Hash hash_type;
    //基本的构造函数

    threadsafe_lookup_table(unsigned num_buckets=19,
                            Hash const& hasher_=Hash()):
                            buckets(num_buckets),
                            hasher(hasher_)
    {
        for(unsigned i=0;i<num_buckets;++i)
        {
            buckets[i].reset(new bucket_type);
        }
    }
    threadsafe_lookup_table(threadsafe_lookup_table const& other)=delete;
    threadsafe_lookup_table& operator=(threadsafe_lookup_table const& other)=delete;
    //根据关键字查找值

    Value value_for(Key const& key,
                    Value const& default_value=Value()) const
    {
        return get_bucket(key).value_for(key,default_value);
    }
    
    void add_or_update_mapping(Key const& key,Value const& value)
    {
        get_bucket(key).add_or_update_mapping(key,value);
    }
    void remove_mapping(Key const& key)
    {
        get_bucket(key).remove_mapping(key);
    }
};

```

#### 6.3.2  编写一个使用锁的线程安全链表

链表的基本功能：

链表的基本操作

- 向列表添加一个元素
- 当某个条件满足时,就从链表中删除某个元素
- 当某个条件满足时,从链表中查找某个元素
- 当某个条件满足时,更新链表中的某个元素
- 将当前容器中链表中的每个元素,复制到另一个容器中

线程安全的迭代器

```c++
//定义模板类


template<typename   T>
class threadsafe_list
{
    //链表数据节点

    struct node
    {
        std::mutex m;
        std::shared_ptr<T> data;
        std::unique_ptr<node> next;
        //构造函数

        node():next(){}
        //数值构造函数

        node(T const& value):data(std::make_shared<T>(value)){}
    };
    //定义头部节点

    node head;
public:
        threadsafe_list(){}
        ~threadsafe_list()
        {
            remove_if([](node const&){return true;});
        }
        threadsafe_list(threadsafe_list const&  other)=delete;
        threadsafe_list& operator=(threadsafe_list const& other)=delete;
        //从头部插入

        void push_front(T const& value)
        {
            //创建新节点
            std::unique_ptr<node> new_node(new node(value));
            //头部节点加锁

            std::lock_guard<std::mutex> lk(head.m);
            new_node->next=std::move(head.next);
            head.next=std::move(new_node);
        }
        //定义迭代函数

        template<typename Function>
        void for_each(Function f)
        {
            node* current=&head;
            std::unique_lock<std::mutex> lk(head.m);
            //便利链表

            while(node* const next=current->next.get())
            {
                //保护下一个节点数据

                std::unique_lock<std::mutex> next_lk(next->m);
                //上一个节点解锁

                lk.unlock();
                //执行函数

                f(*next->data);
                //更改当前指针

                current=next;
                //移动对象

                lk=std::move(next_lk);
            }
        }
        //查找一个条件的元素
        //定义查找关键函数模板

        template<typename Predicate>
        std::shared_ptr<T> find_first_if(Predicate p)
        {
            node* current=&head;
            std::unique_lock<std::mutex> lk(head.m);
            while(node* const   next=current->next.get())
            {

                std::unique_lock<std::mutex> next_lk(next->m);
                lk.unlock();
                if(p(*next->data))
                {
                    return next->data;
                }
                current=next;
                lk=std::move(next_lk);
            }
            return std::shared_ptr<T>();
        }
        //按照条件删除元素

        template<typename Predicate>
        void remove_if(Predicate p)
        {
            node* current=&head;
            std::unique_lock<std::mutex> lk(head.m);
            while(node* const next=current->next.get())
            {
                std::unique_lock<std::mutex> next_lk(next->m);
                //是否符合查找条件

                if(p(*next->data))
                {
                    std::unique_ptr<node> old_next=std::move(current->next);
                    current->next=std::move(next->next);
                    next_lk.unlock();
                }else{
                    //解锁下一个

                    lk.unlock();
                    //移动当前指针

                    current=next;
                    //移动下一个锁

                    lk=std::move(next_lk);
                }
            }
        }
};

```