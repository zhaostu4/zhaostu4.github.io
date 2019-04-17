---
layout:     post
title:      C++ Primer 学习笔记
subtitle:   C++ Primer 学习记录
date:       2019-4-17
author:     王鹏程
header-img: img/post-bg-ios10.jpg
catalog: true
tags:
    - C++
    - 基础编程
---

# C++ Primer学习笔记
> 2019-4-17 12:31:29 编写记录如下；

## 第1章 开始
### std::cin 中的循环流使用
_参考链接：_[while(cin >> num)循环输入问题](https://bbs.csdn.net/topics/390809866)[C++ cin输入流 详细用法](https://blog.csdn.net/imkelt/article/details/51925479);

C++ 中可以使用`while(std::cin >> value){//Code }`的方式来进行循环数据的读入，直到没有输出为止；示例代码如下：
```c++
/*
输入样例： 3 4 5 6
输出 ： Sum is :18
 */
#inlude <iostream>
int main()
{
    int sum =0;value=0;
    //循环读取数据
    while(std::cin>>value){
        sm+=value;
    }
    std::out<<"Sum is:"<<sum<<std:endl;
    return 0;
}
```
注意：

* 当键盘向程序中输入数据时，对于文件结束；Windows是`Ctrl+Z`然后 `Enter`或者`Return`;UNIX 中是`Ctrl+D` 然后再加`enter`;
* 当缓冲区中有残留数据时，`std::cin`会直接去读取缓冲区的数据而不会请求键盘输入。重要的是，回车符也会被存在输入缓冲区中。
* 当程序中有多个等待循环输入时，需要使用`cin.clear()`来重置循环状态，方便再次输入; 

## C++变量和基本类型
_参考链接：_[C/C++中基本数据类型所占内存大小](https://blog.csdn.net/zcyzsy/article/details/77935651);[C/C++中基本数据类型在不同系统中所占空间大小](https://blog.csdn.net/yimingsilence/article/details/54730438);
C++中定义了算术类型(arithmetic type)和空类型(void)在内的基础数据结构算术类型表如下：

| 类型 | 含义 | 最小尺寸 |
|:------------:|:---------------:|:-----:|
| bool         | 布尔类型| 未定义 |
| char | centered | 8位 |
| wchart_t | 宽字符 | 16位 |
| chart16_t | Unicode字符 | 16位 |
| chart32_t | Unicode字符 | 32位 |
| short | 短整型 | 16位 |
| int | 整型 | 16位 |
| long | 长整型 | 32位 |
| long long | 长整型 | 64位 |
| float | 单精度浮点数 | 6位有效数字 |
| double | 双精度浮点数 | 10位有效数字 |
| long double | 扩展精度浮点数 | 10位有效数字 |

注意：

* 关于不同类型，字节内存分配的问题，不同的操作系统存在不同的内存分配策略；因此不一定按照上面的进行分配；详细内容参看参考链接。
* 对于C++中的字节对齐内容需要重点考虑([C++ 字节对齐的总结(原因和作用)](https://blog.csdn.net/sweetfather/article/details/78487563);[C/C++ 字节对齐](https://blog.csdn.net/chengonghao/article/details/51674166));