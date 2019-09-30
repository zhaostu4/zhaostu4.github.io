---
layout:     post
title:      Linux内核设计与实现 学习笔记 (一)
subtitle:   Linux内核设计与实现 学习笔记 (一)
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

> 2019-09-29 19:39:56

# Linux内核设计与实现(第三版) 学习笔记

_参考连接：_

## 第1章 Linux 内核简介

linux内核版本号如下：

![linux主要版本号](../img/2019-09-29-21-38-08.png)

## 第 2 章 从内核出发

可以从[内核网站](https://www.kernel.org/)中下载Linux的内核源码。可以使用`uname -r`查看内核release版本号。

也可以使用如下命令查看最新的版本源代码

```bash
git clone git://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux-2.6.git
#更新到最新版本
git pull
```
也可以直接使用如下命令直接clone最新版本。然后使用`git checkout`来切换分支

```bash
 git clone git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
```

#### 2.1.3 使用补丁

Linux中一般使用补丁的形式对发布的代码进行修改，增量补丁可以作为版本转移的桥梁。使用如下代码在内部源码树开始进行增量补丁的添加

```bash
patch -p1 < .../patch-x.y.z
```

### 2.2 内核源码树

内核雨那么树由很多目录组成，而大多数目录又包含更多的子目录。源码树的根目录及其子目录如下表所示：

|目录|描述|
|:------|:--------|
|arch|特定体系结构的源码如：ARM，AMD64|
|block|块设备I/O层|
|crypto|加密API|
|Documentation|内核源码文档|
|drivers|设备驱动程序|
|firmware|使用某些驱动程序而需要的设备的固件|
|fs|VFS和各种文件系统|
|include|内核头文件|
|init|内核引导和初始爱护|
|ipc|进程间通信代码|
|kernel|调度程序核心子系统|
|lib|通用内核函数|
|mm|内存管理子系统和VM|
|net|网络子系统|
|samples|示例，示范代码|
|scripts|编译内核所用的脚本|
|security|Linux安全模块|
|sound|语音子系统|
|usr|早期用户空间代码(所谓的initramfs)|
|tools|在Linux开发中有用的工具|
|virt|虚拟化基础结构|

COPYING是内个许可证，CREDITS是内核开发者列表。MAINTAINERS是维护者列表

### 2.3 编译内核

一般使用make命令来进行

```bash
#使用命令行选项

make menuconfig
#基于gtk+的图形工具

make gconfig
#使用默认体系结构配置

make defconfig

```

也可以使用如下的命令，将电脑中已有的配置文件作为编译配置选项

```bash
#将配置文件内容，添加到.config文件中

zcat /proc/config.gz > .config
#使用配置文件编译

make oldconfig

make 进行编译
```

### 2.3.2 减少编译的垃圾信息

```bash
#使用如下命令，进行垃圾信息重定向到文件

make > .. /detritus
#直接扔掉信息

make > /dev/null
```

接下来运行如下命令，进行内核模块的安装
```bash
make modules_install
```

### 2.4 内核开发的特点

- 内核编程时既不能访问c库也不能访问标准的c头文件:
  - 完整的C库太大太低效，因此不方便使用
  - 基本头文件在顶级目录下的include目录中，体系结构相关的头文件在arch/<architecture>/include/asm目录下使用`#include <asm/ioctl.h>`进行头文件的包含。
- 内核编程时必须使用GNU C((GNU C 、ANSI C、标准C、标准c++区别和联系)[http://blog.sae.sina.com.cn/archives/749])
  - GNU C 支持inline内联函数
  - 使用`asm volatile("rdtsc":"=a" (low),"=d" (high))`嵌入汇编代码
- 内核编程时缺乏像用户空间那样的内存保护机制
  - 用户进程中进行非法内存访问，内核会发现错误并发送SIGSEGV信号，结束整个进程。内核自己访问非法内存往往会造成内核死去。
  - 内核中的内存都不分页，没用掉一个字节，物理内存就减少一个字节
- 内核编程时难以执行浮点运算
  - 用户空间中的浮点操作，内核会完成浮点数到整数的转换。因此内核并不能完美的支持浮点数操作，因为它本身不能陷入
- 内核给每个进程只有一个很小的定长堆栈
- 内核支持异步中断、抢占和SMP(対称多处理器结构)，因此必须时刻注意同步和并发
- 要仔细考虑可移植的特性