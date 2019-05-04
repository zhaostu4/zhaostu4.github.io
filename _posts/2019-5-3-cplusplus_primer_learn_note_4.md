---
layout:     post
title:      C++ Primer 学习笔记 (三)
subtitle:   C++ Primer 学习记录 (三)
date:       2019-5-4
author:     王鹏程
header-img: img/post-bg-miui6.jpg
catalog: true
tags:
    - C++
    - 基础编程
---
> 2019-5-4: 16:03

## 类

### 定义抽象数据
c++中类的实质就是数据的抽象实现和封装。抽象数据依赖于接口和实现，分离的编程技术；**定义在函数内部的函数是隐式的inline函数**

#### 类中的this关键字
_参考链接：_ [C++类中this指针的理解](https://www.cnblogs.com/liushui-sky/p/5802981.html); [C++ this 指针](https://www.runoob.com/cplusplus/cpp-this-pointer.html);[关于this指针](https://blog.csdn.net/shidantong/article/details/80104710);

首先声明`this`指针是c++为了方便管理类中的函数而产生的，每个对象的`this`指针都指向对象本身，可以使用对象的非静态方法。`this`指针是编译器给予的每个成员函数的隐含参数，用来指向调用对象。
注意：

1. 友元函数没有 this 指针，因为友元不是类的成员。只有成员函数才有 this 指针。
2. this指针其本身的内容是不能被改变的，其类型为：**类类型 * const**
3. this指针不是对象本身的一部分，不影响该对象的大小
4. this指针的作用域在类的 **非静态成员函数**的内部，只能在其内部进行使用，其他任何函数都不能，静态成员函数内部无this指针，后面会详述。
5.  this指针是类中非静态成员函数的第一个默认隐含参数，编译器自动传递和维护，用户不可显示传递
6.  this指针可以为空但是类内部的成员变量数据不能为空，因此当调用使用成员变量时，请确保成员变量不为空。

函数调用约定如下：

![函数调用约定表](https://img-blog.csdn.net/20180426163408707?watermark/2/text/aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3NoaWRhbnRvbmc=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70)

_thiscall关键调用：

- 它只能用在类的成员函数上
- 参数从右向左进行压栈
- 若参数个数确定，this指针通过ecx寄存器传递给被调用者；若参数不确定，this指针在所有参数被压栈后压入堆栈
- 对于参数不确定的函数，调用者清理堆栈，否则函数自己清理堆栈。