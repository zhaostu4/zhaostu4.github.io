---
layout:     post
title:      STL 源码剖笔记(四)
subtitle:   STL 源码剖析笔记(四)
date:       2019-07-06
author:     王鹏程
header-img: img/post-bg-ios10.jpg
catalog: true
tags:
    - C++
    - 基础编程
    - STL源码解析
---

> 2019-08-04 21:47:52


## 第七章 仿函数

仿函数也叫对象函数；在STL提供的各种算法中，都允许用户自定义相关算法。以结果的false或者true来进行相关的排序操作，用来执行函数的就是仿函数。一般步骤是先设计一个函数，在将函数的相关指针指向函数对应的结果
