---
layout:     post
title:      Linux程序设计 学习笔记 (五)
subtitle:   Linux程序设计 学习笔记 (五)
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

> 2019-09-23: 16:48:53

# Linux 程序设计 阅读笔记(五)

## 参考链接：

- [Linux内核文档首页](https://www.kernel.org/doc/Documentation/)
- [Linux文档](https://linux.die.net/)
- [Linux c 开发手册](https://legacy.gitbook.com/book/wizardforcel/linux-c-api-ref/details)
- [Linux Kernel API](https://www.kernel.org/doc/htmldocs/kernel-api/index.html)
- [书中代码地址](http://www.wrox.com/WileyCDA/WroxTitle/Beginning-Linux-Programming-4th-Edition.productCd-0470147628,descCd-DOWNLOAD.html)
- [POSIX thread (pthread) libraries](https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html)

## 第 15 章 嵌套字(socket)

socket是管道概念的一个廓镇。使用与管道类似的方法来使用嵌套字，嵌套字中还包括了计算机网络中的通信。
本章主要内容：

- 嵌套字链接的工作原理
- 嵌套字的属性、地址和通信
- 网络信息和互联网守护进程(inetf/xinetd)
- 客户和服务器

### 15.1 什么是嵌套字

嵌套字(socket)是一种通信机制，客户端/服务器系统的开发工作既可以在本地进行，也可以跨网络进行。socket明确的将客户端与服务器区分开来，这也是socket区别于管道通信的地方。

### 15.2 socket连接

1. 服务器应用程序用系统调用socket来创建一个socket嵌套字。它是系统分配个该服务器进程的类似文件描述符的资源，不能与其它进行共享。
2. 服务器会给socket起一个名字，本地socket名称是Linux文件系统中的文件名。一般放在/tmp或者/usr/tmp目录中。Linux将进入的特定端口号的连接转接到正确的拂去其进程。服务器系统使用bind来给嵌套字命名，然后服务器进程就开始等待客户连接到命名嵌套字。系统使用listen创建一个队列并将其用于存放来自客户端的接入连接。条用accept来接受客户端的连接。
3. 服务器调用accept时，新建一个与特定客户端相关的新的嵌套字来方便通信。
