---
layout:     post
title:      NVCC 学习笔记
subtitle:   NVCC 学习笔记
date:       2019-4-17
author:     王鹏程
header-img: img/post-bg-ios10.jpg
catalog: true
tags:
    - C/C++
    - complier
    - CUDA
    - NVCC
---

# NVCC学习笔记
_参考链接:_ [NVCC Introduction](https://docs.nvidia.com/cuda/cuda-compiler-driver-nvcc/);[nvcc编译器参数](https://blog.csdn.net/fb_help/article/details/79289212);
>2019-1-17  

## CUDA编译流程简介
_参考链接：_ [NVCC CUDA编译流程](https://blog.csdn.net/slow_jiulong/article/details/53157684);
Nvcc是一种编译器驱动，通过命令行选项可以在不同阶段启动不同的工具完成编译工作，其目的在于隐藏了复杂的CUDA编译细节，并且它不是一个特殊的CUDA编译驱动而是在模仿一般的通用编译驱动如gcc，它接受一定的传统编译选项如宏定义，库函数路径以及编译过程控制等。所有非CUDA编译步骤扔给通用的C编译器，在Windows下是MS的"cl"编译器，而在Linux下是gcc。CUDA程序编译的路径会因在编译选项时设置的不同CUDA运行模式而不同，如模拟环境的设置等。nvcc封装了四种内部编译工具:

- nvopencc : (C:\CUDA\open64\bin)
- ptxas 
- fatbin
- cudafe

### NVCC 编译流程
1. 预处理：
输入的cu文件有一个预处理过程，这一过程包括的步骤有将该源文件里的宏以及相关引用文件扩展开，然后将预编译已经产生的与C有关的CUDA系统定义的宏扩展开，并合并分支编译的结果。CUDA源文件经过预处理过程后会生成具有.cup后缀名的经过预处理的源文件，经过预处理的源文件是再现程序bug的最好形式。通常，这个预处理过程是隐式完成的，如果想要显示执行，可以用命令`nvcc -E x.cu -o x.cup`或`nvcc -E x.cu > x.cup`来实现。
2. 前后端设备分离：
将预处理的结果送给CUDA前端，即CUDAfe。通过CUDAfe分离源文件，然后调用不同的编译器分别编译。cudafe被称为CUDA frontend，会被调用两次，完成两个工作：一是将主机代码与设备代码分离，生成gpu文件，二是对gpu文件进行dead code analysis，传给nvopencc，未来的版本可能没有第二次调用。 Nvopencc生成ptx文件传给ptxas，最后将cubin或ptx传给fatbin组合成一个设备代码描述符。
3. 生成编译代码：
编译阶段CUDA源代码对C语言所扩展的部分将被转成regular ANSI C的源文件，也就可以由一般的C编译器进行更多的编译和连接。也即是设备代码被编译成ptx（parallel thread execution）代码和/或二进制代码，host代码则以C文件形式输出，在编译时可将设备代码描述符链接到所生成的host代码，将其中的cubin对象作为全局初始化数据数组包含进来，但此时，kernel执行配置也要被转换为CUDA运行启动代码，以加载和启动编译后的kernel。在调用CUDA驱动API时，CUDA运行时系统会查看这个描述符，根据当前的GPU load一块具有合适ISA的image，然后便可单独执行ptx代码或cubin对象，而忽略nvcc编译得到的host代码。
Nvcc的各个编译阶段以及行为是可以通过组合输入文件名和选项命令进行选择的。它是不区分输入文件类型的，如object, library or resource files，仅仅把当前要执行编译阶段需要的文件传递给linker。

### NVCC编译过程和兼容性详解
_参考链接：_ [NVCC编译过程和兼容性详解](https://blog.csdn.net/fb_help/article/details/80462853);

CUDA编译阶段将源文件中包含C++ externed（cuda 相关代码）的内容变换到标准的C++代码，然后交给c++编译器进行进一步的编译和链接。如图：

![NVCC编译流程图](img/cuda_complie_flow.png);

