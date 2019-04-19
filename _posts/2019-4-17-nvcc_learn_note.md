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

![NVCC编译流程图](https://docs.nvidia.com/cuda/cuda-compiler-driver-nvcc/graphics/cuda-compilation-from-cu-to-executable.png)

NVCC主要编译流程上文中有简单描述；主要注意的是fatbinary，fatbinary，fat是肥的意思，（这里简单讲解，下文会有深入的理解），为了让应用程序适应不同的GPU，fatbinary里可能会有多种GPU的实现，程序在运行的时候会根据自己的特点选择合适的最高效的GPU实现进行运行： 
CUDA 运行时系统（GPU驱动程序）会监视fatbinary文件中的内容，每次程序运行时，CUDA 运行时系统（GPU驱动程序）都会找到fatbinary中最合适部分并映射到当前GPU。（fatbinary会有适合不同的GPU的实现）；
### GPU 中的“代”
GPU中的代是指NVIDIA GPU架构和计算能力的评价标准，如sm_30、sm_70，sm_50等；分别对应不同的GPU架构；算能力高的GPU可以运行编译成低代的程序，反之则不行，如计算能力6.1的GPU可以运行编译成compute_30，sm_30的程序）；一个GPU代中的二进制兼容性可以在某些条件下得到保证，因为它们共享基本的指令集。 两个GPU版本之间的情况就是这样，它没有功能上的差异（例如，当一个版本是另一个版本的缩小版本时），或者一个版本在功能上被包含在另一个版本中。 后者的一个例子是基础的Kepler版本sm_30，其功能是所有其他Kepler版本的子集：针对sm_30编译的任何代码将在所有其他Kepler GPU上运行。
#### GPU中的小“代” 
除了sm_20，sm_30，sm_50，sm_60这些大的代号，还有sm_21, sm_35, sm_53 ,sm_61这些小代，这些小代不会做大的改变，会有一些小的调整，如调整寄存器和处理器集群的数量，这只影响执行性能，不会改变功能。程序更精确的对应GPU代号可能可以达到最佳性能。

### GPU中的应用程序兼容性
>  --两端式编译结构，真实GPU与虚拟GPU；

- CUDA程序的编译必须经历两个过程，即虚拟框架和真实框架，虚拟框架决定了程序最小的可运行GPU框架，而真实框架决定了程序可运行的最小的实际GPU。 
例如-arch=compute_30;-code=sm_30表示计算能力3.0及以上的GPU都可以运行编译的程序。但计算能力2.0的GPU就不能运行了。
- 即时编译（Just-In-Time）机制让程序可以在大的GPU框架内动态选择与电脑GPU最合适的小代。
- –generate-code保证用户GPU可以动态选择最适合的GPU框架（最适合GPU的大代和小代）。

CPU中不同代CPU应用程序的兼容性很好，已发布的指令集体系结构是确保当这些分布式应用程序成为主流时能够继续在新版CPU上运行的常用机制。这种情况对于GPU而言是不同的，因为NVIDIA不能保证二进制兼容性，nvcc依靠两阶段编译模型来确保应用程序与未来GPU世代的兼容性。即虚拟架构和真实架构：虚拟架构确定编译成的代号的功能，真实架构确定编译成的真实代号的功能和性能。

![GPU虚拟架构图](https://img-blog.csdn.net/2018052616211679?watermark/2/text/aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2ZiX2hlbHA=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70)

虚拟GPU与真实GPU之间向上兼容；当虚拟GPU版本低于真实GPU时程序能够正常运行；NVIDIA中即时编译（JIT）能够好的体现兼容性。虚拟GPU架构特性表如下：

| 虚拟列表 | 添加的新特性 |
|:------------:|:------------:|
|compute_30 and compute_32|Basic features,Kepler support,Unified memory programming|
|compute_35| Dynamic parallelism support|
|compute_50, compute_52, and compute_53|Maxwell support|
|compute_60, compute_61, and compute_62|Pascal support |
|compute_70 and compute_72 | Volta support |
|compute_75| Turing support |

NVIDIA GPU真实架构如下：

|真实的架构|功能更新|
|:-------:|:-----:|
|sm_30 and sm_32|Basic features,Kepler support|
|sm_35|Dynamic parallelism support|
|sm_50, sm_52, and sm_53|Maxwell support|
|sm_60, sm_61, and sm_62|Pascal support|
|sm_70|Volta support|
|sm_75|Turing support|

NVCC在提高兼容性的处理方式上采用了两种机制：即时编译(JIT)和fatbinaries
#### 即时编译(Just-In-Time)
过指定虚拟代码架构而不是真实的GPU，nvcc推迟PTX代码的组装，直到应用程序运行时（目标GPU完全已知）。 例如，当应用程序在sm_50或更高版本的架构上启动时，下面的命令允许生成完全匹配的GPU二进制代码。 
```shell
nvcc xx.cu –gpu-architecture=compute_50 –gpu-code=compute_50 
```
即使编译，即程序运行时，再根据当前的GPU编译成自己计算能力动态编译成应用程序。这就可以让GPU选择想要的版本进行编译。即双compute_的组合。但这种只能保证同一代的兼容性。
注意：当GPU计算能力低于编译的虚拟框架时，JIT将失败。

#### Fatbinaries
在JIT中克服启动延迟，同时仍允许在新GPU上执行的另一种解决方案是指定多个代码实例，如nvcc x.cu –gpu-architecture = compute_50 –gpu-code = compute_50，sm_50，sm_52该命令为两个Kepler变体生成精确代码，以及在遇到下一代GPU时由JIT使用的PTX代码。nvcc将其设备代码组织在fatbinaries中，这些代码能够保存相同GPU源代码的多个翻译。在运行时，CUDA驱动程序将在设备功能启动时选择最合适的翻译。
即一次保存多个精确的真实框架的二进制结果，当程序被传给GPU时，GPU选择最好的结果，因为一次性加入了多个真实框架，所以被称为‘fat‘。但这也仅仅是保证了大代之间的兼容性。
在编译参数选择时也可以使用`-generate-code`参数，他会在编译时产生不同代的PTX再配合JIT或者fatbinary实现所有GPU兼容。因此在使用cuda程序兼容性的时候，指定虚拟架构决定cuda运行下限，再通过指定`-generate-code`或者`-gpu-code=xxx,xxxx,xxxx,...`实现程序的兼容性；

## NVCC 参数解析
_参考链接：_ [CUDA_Compiler_Driver_NVCC](https://docs.nvidia.com/cuda/pdf/CUDA_Compiler_Driver_NVCC.pdf);

---

### C++ NVCC编译参数关键字

#### NVCC关键字有如下:

|关键字|作用描述|
|:------:|:------:|
|`__NVCC__`|Defined when compiling C/C++/CUDA source files|
|`__CUDACC__`|Defined when compiling CUDA source files|
|`__CUDACC_RDC__`|Defined when compiling CUDA sources files in relocatable device code mode|
|`__CUDACC_DEBUG__`|Defined when compiler CUDA source files in the device-debug mode|
|`__CUDACC_RELAXED_CONSTEXPR__`|Defined when the `--expt-relaxed-constexpr` flag is specified on the command line|
|`__CUDACC_EXTENDED_LAMBDA__`|Defined when the `--expt-extended-lambda` flag is specified on the command line|
|`__CUDACC_VER_MAJOR__`|Defined with the major version number of `nvcc`|
|`__CUDACC_VER_MINOR__`|Defined with the minor version number of `nvcc`|
|`__CUDACC_VER_BUILD__`|Defined with the build version number of `nvcc`|

#### NVCC中文件类型和解析：

|Input File Prefix  | Description|
|:------:|:------:|
|`.cu` | CUDA source file, containing host code and device functions|
|`.c` | C source file|
|`.cc`, `.cxx`, `.cpp` |C++ source file|
|`.ptx`  |  PTX intermediate assembly file|
|`.cubin`|  CUDA device code binary file (CUBIN) for a single GPU architecture |
|`.fatbin` | CUDA fat binary file that may contain multiple PTX and CUBIN files |
|`.o`, `.obj` |   Object file |
|`.a`, `.lib` |   Library file |
|`.res`  |  Resource file |
|`.so` |Shared object file|

#### NVCC 编译选项

|Phase|Long Name|Short Name|Default Output File Name|
|:------:|:------:|:------:|:------:|
|CUDA compilation to C/C++ source file  | `--cuda`  `-cuda`  | `.cpp.ii` appended to source file name, as in `x.cu.cpp.ii.` This output file can be compiled by the host compiler that was used by nvcc to preprocess the `.cu`file. |
|C/C++ preprocessing | `--preprocess`  |  `-E` | <result on standard output> |
 |C/C++ compilation to object file  |  `--compile`  | `-c` | Source file name with suffix replaced by o on Linux and Mac OS X, or obj on Windows |
 |Cubin generation from CUDA source files | `--cubin` | `-cubin` |  Source file name with suffix replaced by `cubin` |
 |Cubin generation from PTX intermediate files. |  `--cubin` | `-cubin` |  Source file name with suffix replaced by cubin |
 |PTX generation from CUDA source files |  `--ptx` |  `-ptx` | Source file name with suffix replaced by `ptx` |
 |Fatbinary generation from source, PTX or cubin files  |  `--fatbin` | `-fatbin` | Source file name with suffix replaced by fatbin |
 |Linking relocatable device code.  |  `--device-link`  | `-dlink` |  `a_dlink.obj` on Windows or `a_dlink.o` on other platforms |
 |Cubin generation from linked relocatable device code.   | `--device-link--cubin`  |  `-dlink-cubin`  |  `a_dlink.cubin` |
 |Fatbinary generation from linked relocatable device code  |  `--device-link--fatbin` |  `-dlink-fatbin`  | `a_dlink.fatbin` |
 |Linking an executable  |  <no phase option> | |  `a.exe` on Windows or `a.out` on other platforms |
 | Constructing an object file archive, or library | `--lib` |  `-lib`  |    `a.lib` on Windows or `a.a` on other platforms |
 |make dependency generation | `--generate-dependencies` | `-M` | <result on standard output> |
 |make dependency generation without headers in system paths. | `--generate-nonsystem-dependencies` |  `-MM`  | <result on standard output> |
 |Running an executable  | `--run` |  `-run` | |    

 ### MVCC 命令选项；
 NVCC 命令工具使用方法和参数解析详见`nvcc --help`; 
 #### 常用参数和功能如下：

 |全称| 简写 | 作用|
 |:------:|:------:|:------:|
 |`--output-file [file]`| `-o [file]` | 输出目标文件 |
 |`--pre-include [file1,file2,...] `| `-inlude [file1,file2,...]` | 添加头文件 |
 |`--library [library]`| `-l [library]` | 添加动态链接文件 |
 |`--define-macro def`,...| `-D` | Specify macro definitions for use during preprocessing or compilation. |
| `--undefine-macro def`,...  | `-U` | Undefine macro definitions during preprocessing or compilation. |
| `--include-path path`,... | `-I` |  Specify include search paths. |
| `--system-include path`,... |  `-isystem`  |  Specify system include search paths. |
| `--library-path path`,... | `-L` | Specify library search paths (see Libraries).|
| `--output-directory directory`  |  `-odir` |   Specify the directory of the output file. This option is intended for letting the dependency generation step (see `--generate-dependencies`) generate a rule that defines the target object file in the proper directory. |
| `--compiler-bindir directory` | `-ccbin` | Specify the directory in which the compiler executable resides. The host compiler executable name can be also specified to ensure that the correct host compiler is selected. In addition, driver prefix options (`--input-drive-prefix`, `--dependency-drive-prefix`, or `--drive-prefix`) may need to be specified, if nvcc is executed in a Cygwin shell or a MinGW shell on Windows. |
| `--cudart {none|shared|static} ` | `-cudart` | 
Specify the type of CUDA runtime library to be used: no CUDA runtime library, shared/dynamic CUDA runtime library, or static CUDA runtime library. 
Allowed values for this option: `none`, `shared`, `static`. 
Default value: `static` |

| `--libdevice-directory directory` | `-ldir`  |  Specify the directory that contains the libdevice library files when option `--dont-use-profile` is used. Libdevice library files are located in the nvvm/libdevice directory in the CUDA Toolkit. |
