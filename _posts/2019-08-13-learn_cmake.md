---
layout:     post
title:      cmake 学习笔记
subtitle:   cmake 学习笔记 
date:       2019-08-13
author:     王鹏程
header-img: img/post-bg-ios10.jpg
catalog: true
tags:
    - C++
    - 基础编程
    - Linux
    - MakeFile
    - cmake
---

> 2019-08-13 12:58:56

cmake学习笔记

_参考链接：_ [CMake文档](https://mubu.com/docbgFI4BMB6V);[CMake使用教程](https://blog.csdn.net/dabenxiong666/article/details/53998998)

## CMake链接

- [CMake官网](https://cmake.org/)
- [CMakeWiki](https://cmake.org/Wiki/CMake)
- [CMake源码](https://github.com/Kitware/CMake)
- [Learn CMake](https://github.com/Akagi201/learning-cmake)
- [CMake文档](https://cmake.org/cmake/help/latest/manual/cmake.1.html)
- [CMake Tutorial](https://cmake.org/cmake-tutorial/)

# CMake文档

-----

## 1 安装
### 1.1 Windows安装
网址：https://cmake.org/download/
###  1.2 macOS安装

- 网址：https://cmake.org/download/，下载CMake，并正常安装
- 安装完成之后，使用以下指令创建/usr/local/bin下的CMake的软连接
    + `sudo "/Applications/CMake.app/Contents/bin/cmake-gui" --install`
    + 注意：执行此命令的时候确保CMake处于关闭状态重新打开Terminal，即可正常使用 CMake 的各种指令了，也可以在应用程序列表中使用带 GUI 的 CMake 工具。

### 1.3 Linux安装
- 网址：https://cmake.org/download/，下载对应版本的CMake（32位或者64位）
- 将下载的安装包上传到Linux服务器，比如:/root
- 输入以下命令进行解压
    + `tar -zxvf cmake-3.13.0-rc1-Linux-x86_64.tar.gz`
    + 注意：后面是官网下载的对应版本的名字
- 把解压后的目录改名为：cmake
    + `mv cmake-3.10.0-rc4-Linux-x86_64 cmake`
    + 设置环境变量
        * 使用指令`vi .bash_profile`来设置环境变量，找到`PATH=$PATH:$....`这一行，后面添加CMake安装目录里面的bin目录的地址
        * 如果是在/root目录安装的CMake，那添加的目录就是：/root/cmake/bin
    + 安装完毕，环境变量设置成功之后，命令行输入：cmake --version检测是否安装成功
        * 输出：cmake version 3.13，表示安装成功

------
## 2 使用CMake生成项目

### 2.1 使用Windows或者Linux生成项目
- 进入项目目录（CMakeLists.txt所在目录），新建一个build文件夹，因为CMake会产生很多自己的中间文件。
- 执行 ：cmake ../ 就会在build目录产生项目文件，windows下面默认产生vs的项目。
- 如果要产生其他编译器的makefile，就需要使用-G指定编译器
    + `cmake -G "MinGW Makefiles" ../`
    + 可以使用cmake --help 来查看使用的编译器的名字
    + 生成项目工程文件或者makefile之后，就可以使用对应的编译器来编译项目

### 2.2 使用macOS生成项目
- mac下基本操作和windows、Linux相同，不过cmake命令使用的是：cmake .. （没有右斜杠）

### 注意：（默认已经配置好环境变量）

------

## 3 CMake命令行选项的设置

### 3.1 指定构建系统生成器：-G
- 使用：-G 命令可以指定编译器，当前平台支持的编译器名称可以通过帮助手册查看：cmake --help，
    例如：
    `cmake -G "Visual Studio 15 2017" `../ 使用vs2017构建工程

### 3.2CMakeCache.txt文件
- 当cmake第一次运行一个空的构建的时候，他就会创建一个CMakeCache.txt文件，文件里面存放了一些可以用来制定工程的设置，比如：变量、选项等
- 对于同一个变量，如果Cache文件里面有设置，那么CMakeLists文件里就会优先使用Cache文件里面的同名变量。
- CMakeLists里面通过设置了一个Cache里面没有的变量，那就将这个变量的值写入到Cache里面
- 例子：

```cmake
//变量var的值被设置成1024，如果变量var在Cache中已经存在，该命令不会覆盖cac he里面的值

SET（var 1024）
//如果var在Cache中存在，就优先使用Cache里面的值，如果不存在，就将该值写入Cache里面​
   
SET（var 1024..CACHE..）
//无论Cache里面是否存在，都始终使用该值
   
SET（var..CACHE..FORCE）
```

### 3.3 添加变量到Cache文件中：-D
注意：-D后面不能有空格，例如：cmake -DCMAKE_BUILD_TYPE:STRING=Debug
### 3.4 从Cache文件中删除变量：-U
此选项和-D功能相反，从Cache文件中删除变量，支持使用*和？通配符
### 3.5 CMake命令行模式：-E
CMake提供了很多和平台无关的命令，在任何平台都可以使用：chdir, copy,copy_if_different等
可以使用：cmake -E help进行查询
### 3.6 打印运行的每一行CMake
- 命令行选项中：`--trace`，将打印运行的每一行CMake，例如windows下执行: cmake --trace ..
- 命令：`--trace-source="filename"`就会打印出有关filename的执行

### 3.7 设置编译参数
- `add_definitions（-DENABLED）`，当在CMake里面添加该定义的时候，如果代码里面定义了`#ifdef ENABLED #endif`相关的片段，此时代码里面这一块代码就会生效
- add_definitions( “-Wall -ansi –pedantic –g”)：该命令现已经被取代，使用： add_compile_definitions(WITH_OPENCV2)

### 3.8 设置默认值命令：option
option命令可以帮助我们设置一个自定义的宏，如下：

- option(MY-MESSAGE "this is my message" ON)
- 第一个参数就是我们要设置的默认值的名字
- 第二个参数是对值的解释，类似于注释
- 第三个值是这个默认值的值，如果没有声明，CMake默认的是OFF
- 使用：设置好之后我们在命令行去使用的时候，也可以去给他设定值：cmake -DMY-MESSAGE=on ../
- 注意：使用的时候我们应该在值的前面加“D”
- 这条命令可将MY-MESSAGE的值设置为on，通过这个值我们可以去触发相关的判断
    
## 4 CMake基础知识简介

### 4.1 最低版本

每个CMakeList.txt中第一行都会写`cmake_minimum_required(VERSION X.X)`来指定cmake的最低版本，设置cmake版本必须大于X.X，也可以使用`cmake_minimum_required(VERSION X.X...Y.Y)`来设置cmake的版本范围。

#### 4.1.1 判断cmake版本

```makefile 

#检查当前版本是否小鱼3.12是，则更新版本号变量,否则设置版本为3.12

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.12)
endif()
```

### 4.2 设置项目名称

cmake中使用 `project(MyProject[C] [C++])` 不过后面的语言参数经常省略，默认支持所有语言。

执行project()函数就是定义了一个项目名称，并且会生成两个变量`_BINARY_DIR和_SOURCE_DIR`和`PROJECT_BINARY_DIR`者两个变量是cmake中预定义的变量。变量是否相同，关键在于是内部构建还是外部构建：

- 内部构建： `cmake ./ && make`
- 外部构建：`mkdir build && cd build && cmake ../ && make`
当cmake在外部构建时，产生的中间文件和可执行文件并不在同一目录，因此`PROJECT_BINARY_DIR`和`_BINARY_DIR`内容相同，内部构建的时候指向CMakeLists.txt文件的目录，外部构建的，指向target编译的目录。
此外：PROJECT_SOURCE_DIR和_SOURCE_DIR无论内部构建还是外部构建，指向的内容都是一样的，都指向工程的根目录

### 4.3 生成可执行文件

语法：`add_executable(exename srcname)`

- exename:生成的可执行文件的名字
- srcname:以来的源文件

生成exe的名字，并且指出需要的源文件。
获取文件路径中的所有源文件：`aux_sourcr_directory(<dir> <variable>)`例如：`aux_sourcr_directory(. DIR_SRCS)`将当前目录下的源文件名字存放到变量DIR_SRCS里面 ，如果源文件比较多，直接用DIR_SRCS变量即可。`add_executable(Demo ${DIR_SRCS})`生成名为Demo的可执行文件。

### 4.4 生成库文件

cmake中使用`add_library`来实现库文件的生成；
命令：`add_library(libname [SHARED|STATIC|MODULE] [EXCLUDE_FROM_ALL] source1 source2 ... sourceN)`

- libname:生成的库文件的名字
- [SHARED|STATIC|MODULE]：生成库文件的类型（动态库|静态库|模块）
- [EXCLUDE_FROM_ALL]：有这个参数表示该库不会被默认构建​
- source2 ... sourceN：生成库依赖的源文件，如果源文件比较多，可以使用​aux_sourcr_directory命令获取路径下所有源文件，具体章节参见：CMake基础知识简介->生成可执行文件->获取路径中所有源文件

示例：`add_library(ALib SHARE alib.cpp)`

### 4.5 添加头文件目录

命令1： `target_include_directories(<target> [SYSTEM] [BEFORE] <INTERFACE|PUBLIC|PRIVATE> [items1...] [<INTERFACE|PUBLIC|PRIVATE> [items2...] ...])`

示例1：`target_include_directories(RigelEditor PUBLIC ./include/rgeditor)`，表示给RigelEditor 这个子项目添加一个库文件的路径。

命令2：`include_directories([AFTER|BEFORE] [SYSTEM] dir1 [dir2 …])`
参数解析：

- [AFTER|BEFORE]：指定了要添加路径是添加到原有列表之前还是之后 
- [SYSTEM]​：若指定了system参数，则把被包含的路径当做系统包含路径来处理。
- dir1 [dir2 …]​把这些路径添加到CMakeLists及其子目录的CMakeLists的头文件包含项目中相当于g++选项中的-l的参数的作用。

示例2：`include_directories("/opt/MATLAB/R2012a/extern/include")`

注意：两者都是将include目录添加到目标区域中，但是include_directories是对整个项目进行全局添加，target_include_directories是将CMake中针对指定的目标进行添加。

### 4.6 添加链接库文件路径

#### 4.6.1 添加需要的库文件路径

命令1:  `arget_link_libraries(<target> [item1 [item2 [...]]] [[debug|optimized|general] <item>] ...)`
参数解析：

- item: 对应的链接库文件
- debug|optimized|general:调试配置|其它配置|所有配置

示例1：target_link_libraries(MyProject a b.a c.so) 将文件库链接到项目中，target_link_libraries中的库文件的顺序符合gcc/g++链接顺序规则，即：被依赖的库放在依赖其他的库的后面。

命令2：link_libraries
给当前工程链接需要的文件库。

注意：target_link_libraries可以给工程或者库文件设置其需要链接的库文件，而且不需要填写全路径，但是link_libraries只能给工程添加依赖的库，而且必须添加全路径。

#### 4.6.2 添加需要的库文件的目录

link_directories

### 4.7 控制目标属性

以上的几条命令的区分都是：是否带target前缀，在CMake里面，一个target有自己的属性集，如果我们没有显示的设置这些target的属性的话，CMake默认是由相关的全局属性来填充target的属性，我们如果需要单独的设置target的属性，需要使用命令：set_target_properties()

命令格式：
`set_target_properties(target1 target2 ... PROPERTIES 属性名称1 值 属性名称2 值 ... )`

- 控制编译选项的属性是：COMPILE_FLAGS
- 控制链接选项的属性是：LINK_FLAGS
- 控制输出路径的属性：EXECUTABLE_OUTPUT_PATH（exe的输出路径）、LIBRARY_OUTPUT_PATH（库文件的输出路径）

示例：
```makefile
set_target_properties(exe 
    PROPERTIES 
    LINK_FLAGS -static 
    LINK_FLAGS_RELEASE -s 
    )

```
这条指令会使得exe这个目标在所有的情况下都采用-static选项，而且在release build的时候-static -s 选项。但是这个属性仅仅在exe这个target上面有效。

### 4.8 变量和缓存

#### 4.8.1 局部变量
CMakeLists.txt相当于一个函数，第一个执行的CMakeLists.txt相当于主函数，正常设置的变量不能跨越CMakeLists.txt文件，相当于局部变量只在当前函数域里面作用一样。但是当使用include()直接进行.cmake文件的引入时，可以实现变量的通用引用。

#### 4.8.2 缓存变量

缓存变量就是cache变量，相当于全局变量，都是在第一个执行的CMakeLists.txt里面被设置的，不过在子项目的CMakeLists.txt文件里面也是可以修改这个变量的，此时会影响父目录的CMakeLists.txt，这些变量用来配置整个工程，配置好之后对整个工程使用。
设置缓存变量:
`set(MY_CACHE_VALUE "cache_value" CACHE INTERNAL "THIS IS MY CACHE VALUE")`；//THIS IS MY CACHE VALUE，这个字符串相当于对变量的描述说明，不能省略，但可以自己随便定义。

#### 4.8.3 环境变量

设置环境变量：`set(ENV{variable_name} value)`
获取环境变量：`$ENV{variable_name}`

#### 4.8.4 内置变量

CMake中包含大量的内置变量，和自定义的变量相同，常用的有以下：

- CMAKE_C_COMPILER：指定C编译器
- CMAKE_CXX_COMPILER：指定C++编译器
- EXECUTABLE_OUTPUT_PATH：指定可执行文件的存放路径
- LIBRARY_OUTPUT_PATH：指定库文件的放置路径
- CMAKE_CURRENT_SOURCE_DIR：当前处理的CMakeLists.txt所在的路径
- CMAKE_BUILD_TYPE：控制构建的时候是Debug还是Release命令：set(CMAKE_BUILD_TYPE Debug)
- CMAKE_SOURCR_DIR：无论外部构建还是内部构建，都指的是工程的顶层目录（参考project命令执行之后，生成的_SOURCR_DIR以及CMake预定义的变量PROJECT_SOURCE_DIR）
- CMAKE_BINARY_DIR：内部构建指的是工程顶层目录，外部构建指的是工程发生编译的目录（参考project命令执行之后，生成的_BINARY_DIR以及CMake预定义的变量PROJECT_BINARY_DIR）
- CMAKE_CURRENT_LIST_LINE：输出这个内置变量所在的行。

CMake中的缓存大多数存储在CMakeCache.txt文件文件中。

## 5 CMake基本控制语法

### 5.1 if

基本语法：`if (expression)    COMMAND1(ARGS ...)    COMMAND2(ARGS ...)    ...else (expression)    COMMAND1(ARGS ...)    COMMAND2(ARGS ...)    ...endif (expression)`注意：ENDIF要和IF对应​

- if (expression)，expression不为：空,0,N,NO,OFF,FALSE,NOTFOUND或<var>_NOTFOUND,为真
- IF (not exp)，与上面相反
- if (var1 AND var2)，var1且var2都为真，条件成立
- if (var1 OR var2)，var1或var2其中某一个为真，条件成立
- if (COMMAND cmd)， 如果cmd确实是命令并可调用，为真；
- if (EXISTS dir) 如果目录存在，为真
- if (EXISTS file) 如果文件存在，为真
- if (file1 IS_NEWER_THAN file2)，当file1比file2新，或file1/file2中有一个不存在时为真，文件名需使用全路径
- if (IS_DIRECTORY dir) 当dir是目录时，为真
- if (DEFINED var) 如果变量被定义，为真
- if (string MATCHES regex) 当给定变量或字符串能匹配正则表达式regex时，为真

例：IF ("hello" MATCHES "ell") MESSAGE("true") ENDIF ("hello" MATCHES "ell")

**数字表达式**

- if (var LESS number)，var小于number为真。
- if (var GREATER number)，var大于number为真。
- if (var EQUAL number)，var等于number为真。

**字母顺序比较**

- IF (var1 STRLESS var2)，var1字母顺序小于var2为真。
- IF (var1 STRGREATER var2)，var1字母顺序大于var2为真。
- IF (var1 STREQUAL var2)，var1和var2字母顺序相等为真。

### 5.2 while

WHILE(condition) COMMAND1(ARGS ...) COMMAND2(ARGS ...) ...ENDWHILE(condition)

条件判断参考if

### 5.3 Foreach
FOREACH有三种使用形式的语法，且每个FOREACH都需要一个ENDFOREACH()与之匹配。

**列表循环**

- 语法：`FOREACH(loop_var arg1 arg2 ...) COMMAND1(ARGS ...) COMMAND2(ARGS ...) ...ENDFOREACH(loop_var)`​
- 例子：
```cmake
AUX_SOURCE_DIRECTORY(.SRC_LIST)
FOREACH(F ${SRC_LIST}) 
    MESSAGE(${F})
ENDFOREACH(F)
```
例子中，先将当前路径的源文件名放到变量SRC_LIST里面，然后遍历输出文件名

**循环范围**

- 语法：`FOREACH(loop_var RANGE total) COMMAND1(ARGS ...) COMMAND2(ARGS ...)... ENDFOREACH(loop_var)`
- 例子：

```cmake
​FOREACH(VAR RANGE 100)
    MESSAGE(${VAR})
ENDFOREACH(VAR)​​
```
例子中默认起点为0，步进为1，作用就是输出：0~100。

**范围步进循环**
- 语法： `FOREACH(loop_var RANGE start stop [step]) COMMAND1(ARGS ...) COMMAND2(ARGS ...)... ENDFOREACH(loop_var)`
- 例子：

```cmake
FOREACH(A RANGE 0 100 10)
    MESSAGE(${A})
ENDFOREACH(A)​
```
例子中，起点是0，终点是100，步进是10，输出：0,10,20,30,40,50,60,70,80,90,100。

## 6 构建规范以及构建属性

用于指定构建规则以及程序使用要求的指令：
- target_include_directories()
- target_compile_definitions()
- target_compile_options()

指令格式：

- `target_include_directories(<target> [SYSTEM] [BEFORE]<INTERFACE|PUBLIC|PRIVATE> [items1...] [<INTERFACE|PUBLIC|PRIVATE> [items2...] ...])` 

    主要是设置include的头文件的查找目录，也就是GCC的[-lidr...]选项

- `target_compile_definitions(<target> <INTERFACE|PUBLIC|PRIVATE> [items1...][<INTERFACE|PUBLIC|PRIVATE> [items2...] ...])`

    设置c++文件中预定义的宏变量。
- `target_compile_options(<target> [BEFORE] <INTERFACE|PUBLIC|PRIVATE> [items1...] [<INTERFACE|PUBLIC|PRIVATE> [items2...] ...]`

    gcc其它的一些编译选项，比如-fPIC,-fPIC作用于编译阶段，告诉编译器产生与位置无关代码Position-Independent Code)，则产生的代码中，没有绝对地址，全部使用相对地址，故而代码可以被加载器加载到内存的任意位置，都可以正确的执行。这正是共享库所要求的，共享库被加载时，在内存的位置不是固定的。

以上的额三个命令会生成

- INCLUDE_DIRECTORIES, 
- COMPILE_DEFINITIONS, 
- COMPILE_OPTIONS
变量的值。或者 :
- INTERFACE_INCLUDE_DIRECTORIES
- INTERFACE_COMPILE_DEFINITIONS
- INTERFACE_COMPILE_OPTIONS

这三个命令都有三种可选模式: PRIVATE, PUBLIC。INTERFACE.PRIVATE模式仅填充不是接口的目标属性;INTERFACE模式仅填充接口目标的属性.PUBLIC模式填充这两种的目标属性。

## 7 宏和函数
CMake里面可以定义自己的函数（function）和宏（macro）
函数和宏的区别：

函数是有范围的，而宏没有。如果希望函数设置的变量在函数的外部也可以看见，就需要使用PARENT_SCOPE来修饰，但是函数对于变量的控制会比较好，不会有变量泄露。函数很难将计算结果传出来，使用宏就可以将一些值简单的传出来。

使用示例
- 宏(macro)

```cmake
​macro( [arg1 [arg2 [arg3 ...]]])     
    COMMAND1(ARGS ...)     
    COMMAND2(ARGS ...)     
    ...
endmacro()
```

- 函数(function)

```cmake
function( [arg1 [arg2 [arg3 ...]]])     
    COMMAND1(ARGS ...)     
    COMMAND2(ARGS ...)     
    ...

endfunction()

```
使用示例:
```cmake
macro(macroTest)
    set(test1 "aaa")
endmacro()
​
function(funTest)
    set(test2 "bbb")
endfunction()
​
macroTest()
message("${test1}")
​funTest()
message("${test2}")
​
#最终显示结果为aaa,test2为局部变量，已经超出了他的工作范围。

```

## 8 和其它文件的交互

### 8.1 在代码中使用CMake中定义的变量

使用configure_file命令可以使得在普通文件中也能使用CMake中的变量

语法：

`configure_file(<input> <output> [COPYONLY] [ESCAPE_QUOTES] [@ONLY] [NEWLINE_STYLE [UNIX|DOS|WIN32|LF|CRLF] ])​;`
参数：

- COPYONLY：

   只拷贝文件，不进行任何的变量替换。这个选项在指定了 NEWLINE_STYLE 选项时不能使用（无效）。
- ESCAPE_QUOTES：

    躲过任何的反斜杠(C风格)转义。躲避转义，比如你有个变量在CMake中是这样的 set(FOO_STRING "\"foo\"") 那么在没有 ESCAPE_QUOTES 选项的状态下，通过变量替换将变为 `"foo"`，如果指定了 ESCAPE_QUOTES 选项，变量将不变。
- @ONLY：

    限制变量替换:，让其只替换被 @VAR@ 引用的变量（那么 ${VAR}格式的变量将不会被替换）。这在配置 ${VAR} 语法的脚本时是非常有用的。
- NEWLINE_STYLE:

    指定输出文件中的新行格式。UNIX 和 LF 的新行是 \n ，DOS 和 WIN32 和 CRLF 的新行格式是 \r\n 。 这个选项在指定了 COPYONLY 选项时不能使用（无效）。

### 8.2 在CMake对文件的操作

#### file命令：

- file(WRITE filename "message to write"... ):

    WRITE选项会写一条消息到名为filename中，如果文件存在，则会覆盖原文件，如果文件不存在，他将创建该文件。
- file(APPEND filename "message to write"... ):

    APPEND选项和WRITE选项一样，只是APPEND会写到文件的末尾。
- file(READ filename variable [LIMIT numBytes] [OFFSET offset] [HEX])：

    READ选项会将读取的文件内容存放到变量variable，读取numBytes个字节，从offset位置开始，如果指定了[HEX]参数，二进制代码就会转换为十六进制的转换方式。
- file(STRINGS filename variable [LIMIT_COUNT num] [LIMIT_INPUT numBytes] [LIMIT_OUTPUT numBytes] [LENGTH_MINIMUM numBytes] [LENGTH_MAXIMUM numBytes] [NEWLINE_CONSUME] [REGEX regex] [NO_HEX_CONVERSION])：

    STRINGS标志，将会从一个文件中将ASCII字符串的list解析出来，然后储存在variable 变量中，文件中的二进制数据将会被忽略，回车换行符会被忽略（可以设置NO_HEX_CONVERSION选项来禁止这个功能）。LIMIT_COUNT：设定了返回字符串的最大数量；LIMIT_INPUT：设置了从输入文件中读取的最大字节数；LIMIT_OUTPUT：设置了在输出变量中允许存储的最大字节数；LENGTH_MINIMUM：设置了返回字符串的最小长度，小于该长度的字符串将会被忽略；LENGTH_MAXIMUM设置了返回字符串的最大长度，大于该长度的字符串将会被忽略；NEWLINE_CONSUME：该标志允许新行被包含到字符串中，而不是终止他们；REGEX：指定了返回的字符串必须满足的正则表达式。例如：`file(STRINGS myfile.txt myfile)`，将myfile.txt中的文本按行存储到myfile这个变量中。





