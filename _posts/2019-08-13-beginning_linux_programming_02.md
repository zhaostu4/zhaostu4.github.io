---
layout:     post
title:      Linux程序设计 学习笔记 (二)
subtitle:   Linux程序设计 学习笔记 (二)
date:       2019-08-13
author:     王鹏程
header-img: img/post-bg-ios10.jpg
catalog: true
tags:
    - C/C++
    - Linux
    - 操作系统
    - 程序设计
---

## 第三章 文件操作

### 3.1 linux文件结构
_参考链接：_　[每天进步一点点——Linux中的文件描述符与打开文件之间的关系](https://blog.csdn.net/cywosp/article/details/38965239)；[Linux文件读写机制及优化方式](https://blog.csdn.net/u014743697/article/details/52663975);[【Linux学习笔记】标准IO缓冲：行缓冲、全缓冲、无缓冲](https://blog.csdn.net/LYJwonderful/article/details/80646602)

linux中一切皆是文件。

文件除了本身的文件内容外，还有文件的附属管理信息，这些保存在`inode`节点中。目录是用于保存其他文件的节点号和节点名字的文件，目录文件中的每个数据项都是指向某个文件节点的链接，删除文件名就是删除与之对应的链接。
删除文件时，就是删除了文件的目录项，并且指向该文件的链接数目-1。文件中的数据仍然能够通过其它指向链接访问到。被链接数为0时，才会被正真删除。

