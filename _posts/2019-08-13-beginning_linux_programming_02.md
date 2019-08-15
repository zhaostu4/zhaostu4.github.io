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

linux中一切皆是文件。

文件除了本身的文件内容外，还有文件的附属管理信息，这些保存在`inode`节点中。目录是用于保存其他文件的节点号和节点名字的文件，目录文件中的每个数据项都是指向某个文件节点的链接，删除文件名就是删除与之对应的链接。
删除文件时，就是删除了文件的目录项，并且指向该文件的链接数目-1。