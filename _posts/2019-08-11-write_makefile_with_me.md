---
layout:     post
title:      跟我一起写makefile 学习笔记
subtitle:   跟我一起写makefile 
date:       2019-07-06
author:     王鹏程
header-img: img/post-bg-ios10.jpg
catalog: true
tags:
    - C++
    - 基础编程
    - Linux
    - MakeFil
---

> 2019-08-11 16:04:58

# 跟我一起写makefile

_参考链接：_ [GNU make](http://www.gnu.org/software/make/manual/make.html); [GUN Make指南](https://www.cnblogs.com/itech/archive/2009/09/15/1567001.html)
## 1 概述
make工具：

- Delphi-make
- Visual C++ nmake
- GNU make

文章主要针对GNU的make 

## 2 关于程序的编译与链接

具体的过程可以参照:[C/C++ 的一个符号操作问题](https://wangpengcheng.github.io/2019/05/12/c_peator_problem/)

编译时(.c->.o)：编译器需要的是语法正确，函数与变量的声明的正确。
链接时(.o->exe):主要是链接函数和全局变量。

## 3 Makefile介绍

为了方便使用gcc等编译器进行编译和链接，对复杂项目使用Makefile文件来，记录编译指令的顺序，相当于另外的针对gcc等编译器的shell脚本。使用make命令来执行相关的命令。

### 3.1 Makefile的规则

基本指令格式：

```makefile
target... : prerequisites ...
    command
    ...
    ...

```

target:编译目标文件；可以使用ObjectFile、