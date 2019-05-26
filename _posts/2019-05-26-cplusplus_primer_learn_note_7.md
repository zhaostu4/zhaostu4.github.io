---
layout:     post
title:      C++ Primer 学习笔记(七2)
subtitle:   C++ Primer 学习记录(七)
date:       2019-05-26
author:     王鹏程
header-img: img/post-bg-ios10.jpg
catalog: true
tags:
    - C++
    - 基础编程
---

>>2019-5-26 :19:50:52

## 第 12 章 动态内存

### 12.1 动态内存与智能指针
在c++中动态内存管理是通过一对运算符来完成的：`new`和`delete`;同时为了更加安全的管理指针；标准库提供了两种 **智能指针** ：**shared_ptr**、**unique_ptr** ;前者允许多个指针指向同一个对象；后者独占所指对象。同时还有 **weak_ptr**的伴随类，他是一种弱引用，指向shared_ptr所管理的对象。三种类都定义在头文件memory中

当我们创建指针时，必须提供指针可以指向的类型。

```c++
shared_ptr<string> p1; //shared_ptr,可以指向string 

shared_ptr<list<int>> p2; //shared_ptr,可以指向int的list 
```

**shared_ptr和unique_ptr都支持的操作**

|操作|含义|
|:---|:---|
|`shared_ptr<T> sp unique_ptr<T> up`|空智能指针，可以指向类型为`T`的对象|
|`p`|将p用作一个条件判断，若`p`指向一个对象，则为`true`|
|`*p`|解引用p,获得它指向的对象|
|`p->mem`|等价于`(*p).mem`|
|`p.get()`|返回`p`中保存的指针，要小心使用；若智能指针释放了其对象，返回的指针所指向的对象也就消失了|
|`swap(p,q)/p.swap(q)`|交换`p`和`q`中的指针|

**share_ptr独有的操作**

|操作|含义|
|:---|:---|
|`make_shared<T>(args)`|返回一个`shared_ptr`,指向一个动态分配的类型为`T`的对象。使用`args`初始化此对象|
|`shared_ptr<T> p(q)`|p是`shared_ptr q`的拷贝；此操作会递增`q`中的计数器。`q`中的智能指针必须转化为`*T`|
|`p=q`|p和q都是`shared_ptr`，所保存的指针必须能够相互转换。此操作会递减`p`的引用计数，递增`q`的引用计数；若`p`的引用计数变为0，则将其管理的原内存释放|
|`p.unique()`|若`p.use_count()`为1，返回`true`;否则返回`false`|
|`p.use_count()`|返回与`p`共享对象的智能指针数量；可能很慢，主要用于调试|

注意：
- 我们通常使用`auto`作为`make_shared`指针函数所对应的值。
- 不要使用引用传递，在函数体内对智能指针`shared_ptr`进行赋值操作，计数会-1，离开函数作用域，局部变量销毁，计数可能为0.

计数递增情况：

- 一个`shared_ptr`初始化另外一个`shared_ptr`
- 作为参数传递给一个函数
- 作为函数的返回值

计数减少情况：

- `share_ptr`赋予一个新值
- `shared_ptr`被销毁--局部的`shared_ptr`离开其作用域。

注意：
- 如果将`shared_ptr`存放于一个容器中，而后不再需要全部元素，而只使用其中一部分，要记得`erase()`删除不再需要的那些元素。
- 使用动态内存的一个常见原因是允许多个对象，共享相同的状态。

#### 12.1.2 直接管理内存
c++可以使用`new`和`delete`直接管理内存；尽量在分配内存时，对动态分配内存的对象进行初始化操作。

```c++

int *p1=new int; //如果分配失败，new抛出std::bad_alloc;

int *p2=new (nothrow) int; //如果分配失败，new 返回一个空指针

delete p1,p2;
```
注意： 
- 我们传递给delete的指针必须指向动态分配的内存。或者是一个空指针。
- 不要随便删除一个空指针，可能在指针所指向的内存，被其它程序所使用，删除空指针，会造成极大的问题。
- 动态对象的生存周期是直到被释放为止



