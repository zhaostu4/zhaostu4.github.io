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

## 1 make 的使用

```shell
用法：make [选项] [目标] ...
选项：
  -b, -m                      忽略兼容性。
  -B, --always-make           无条件 make 所有目标。
  -C DIRECTORY, --directory=DIRECTORY
                              在执行前先切换到 DIRECTORY 目录。
  -d                          打印大量调试信息。
  --debug[=FLAGS]             打印各种调试信息。
  -e, --environment-overrides
                              环境变量覆盖 makefile 中的变量。
  --eval=STRING               Evaluate STRING as a makefile statement.
  -f FILE, --file=FILE, --makefile=FILE
                              从 FILE 中读入 makefile。
  -h, --help                  打印该消息并退出。
  -i, --ignore-errors         Ignore errors from recipes.
  -I DIRECTORY, --include-dir=DIRECTORY
                              在 DIRECTORY 中搜索被包含的 makefile。
  -j [N], --jobs[=N]          同时允许 N 个任务；无参数表明允许无限个任务。
  -k, --keep-going            当某些目标无法创建时仍然继续。
  -l [N], --load-average[=N], --max-load[=N]
                              在系统负载高于 N 时不启动多任务。
  -L, --check-symlink-times   使用软链接及软链接目标中修改时间较晚的一个。
  -n, --just-print, --dry-run, --recon
                              Don not actually run any recipe; just print them.
  -o FILE, --old-file=FILE, --assume-old=FILE
                              将 FILE 当做很旧，不必重新生成。
  -O[TYPE], --output-sync[=TYPE]
                              Synchronize output of parallel jobs by TYPE.
  -p, --print-data-base       打印 make 的内部数据库。
  -q, --question              Run no recipe; exit status says if up to date.
  -r, --no-builtin-rules      禁用内置隐含规则。
  -R, --no-builtin-variables   禁用内置变量设置。
  -s, --silent, --quiet       Don not echo recipes.
  -S, --no-keep-going, --stop
                              关闭 -k。
  -t, --touch                 touch 目标而不是重新创建它们。
  --trace                     Print tracing information.
  -v, --version               打印 make 的版本号并退出。
  -w, --print-directory       打印当前目录。
  --no-print-directory        关闭 -w，即使 -w 默认开启。
  -W FILE, --what-if=FILE, --new-file=FILE, --assume-new=FILE
                              将 FILE 当做最新。
  --warn-undefined-variables  当引用未定义变量的时候发出警告。

该程序为 x86_64-pc-linux-gnu 编译
报告错误到 <bug-make@gnu.org>

```

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

#或者

targets : prerequisites ; command
    command
...
```

- target:编译目标文件；可以使用ObjectFile、label或者可执行文件
- prerequisites 生成所需要的文件或者目标
- command 就是make需要执行的命令(任意的shell命令)

注意这里的`command`如果不是和target在同一行，则必须以[Tab]开头。命令行可以使用`\`作为续行符。

简单示例：
```makefile
edit : main.o kbd.o command.o display.o \
    insert.o search.o files.o utils.o
    cc -o edit main.o kbd.o command.o display.o \
    insert.o search.o files.o utils.o
main.o : main.c defs.h
    cc -c main.c
kbd.o : kbd.c defs.h command.h
    cc -c kbd.c
command.o : command.c defs.h command.h
    cc -c command.c
display.o : display.c defs.h buffer.h
    cc -c display.c
insert.o : insert.c defs.h buffer.h
    cc -c insert.c
search.o : search.c defs.h buffer.h
    cc -c search.c
files.o : files.c defs.h buffer.h command.h
    cc -c files.c
utils.o : utils.c defs.h
    cc -c utils.c
clean :
    rm edit main.o kbd.o command.o display.o \
    insert.o search.o files.o utils.o

```
注意这里的clean是一个label相当于一个记录动作的变量

### 3.3 make工作流程

- 查找当前目录下的“Makefile”或者“makefile”文件。
- 找到文件，查找文件中的第一个目标文件(target)。并将其设置为最终的目标文件
- 如果文件不存在，或者相关依赖需要其它的动作生成，或者依赖文件的文件修改时间要比edit这个文件新，则执行后面的命令生成相关目标文件。
- 一层层的查找相关依赖，进行递归，执行程序。

### 3.4 makefile中使用变量

可以直接使用`=`来进行变量赋值。如下：
```makefile
objects = main.o kbd.o command.o display.o \
    insert.o search.o files.o utils.o
edit : $(objects)
    cc -o edit $(objects)
main.o : main.c defs.h
    cc -c main.c
kbd.o : kbd.c defs.h command.h
    cc -c kbd.c
command.o : command.c defs.h command.h

    cc -c command.c
display.o : display.c defs.h buffer.h
    cc -c display.c
insert.o : insert.c defs.h buffer.h
    cc -c insert.c
search.o : search.c defs.h buffer.h
    cc -c search.c
files.o : files.c defs.h buffer.h command.h
    cc -c files.c
utils.o : utils.c defs.h
    cc -c utils.c
clean :
    rm edit $(objects)
```
可以使用`$(变量名)`来对变量进行使用。

### 3.5 makefile 的自动推导

makefile可以自动推导文件，以及文件依赖关系后面的命令，可以使用makefile中的自动推导(看到xxx.o就会将xxx.c文件添加在依赖关系中)。

```makefile

objects = main.o kbd.o command.o display.o \
    insert.o search.o files.o utils.o
edit : $(objects)
    cc -o edit $(objects)
main.o : defs.h
kbd.o : defs.h command.h
command.o : defs.h command.h
display.o : defs.h buffer.h
insert.o : defs.h buffer.h
search.o : defs.h buffer.h
files.o : defs.h buffer.h command.h
utils.o : defs.h
#别名
.PHONY : clean
clean :
    rm edit $(objects)

#也可以这样写

objects = main.o kbd.o command.o display.o \
    insert.o search.o files.o utils.o
edit : $(objects)
    cc -o edit $(objects)
$(objects) : defs.h
  kbd.o command.o files.o : command.h
  display.o insert.o search.o files.o : buffer.h

.PHONY : clean

clean :
    rm edit $(objects)
```

## 4 Makefile总述

Makefile主要包括内容

- 显式规则:直接明文书写的规则
- 隐式规则：自动推导出来的隐晦规则
- 变量的定义:一系列字符串变量的定义
- 文件指示：根据条件的外部makefile的引用导入
- 注释：脚本注释文件

make查找当前目录下的文件顺序是：

-  GNUmakefile
-  makefile
-  Makefile
推荐使用Makefile，最好不要使用GNUMakefile因为这个只能被GNU的make识别。
可以使用`make -f file_name`来指定makefile文件，例如`make -f Make.Linux`。

makefile中可以使用`include`引入其它makefile文件
注意：include前不能加入[Tab]键，可以使用空格

```makefile
bar=a.mk b.mk c.mk e.mk 

include foo.make *.mk $(bar)

```
include的查找路径如下：

- 存在`-I`或者`--include-dir`参数会在这个参数指定的目录下继续寻找
- 存在`<prefix>/include`存在，make也会去寻找。

注意：当没有找到时，make会生成一条警告，并在最终时再次尝试寻找，如果还是没找到就是error。可以使用`-include`告诉mkae **忽略这个的读取错误**，直接继续执行。当然不建议这样做。

**环境变量**

在当前的环境变量中定义了，那么所有的make都会使用，因此建议不要使用。

**make的工作方式**

1. 读入所有的 Makefile 。
2. 读入被 include 的其它 Makefile 。
3. 初始化文件中的变量。
4. 推导隐晦规则,并分析所有规则。
5. 为所有的目标文件创建依赖关系链。
6. 根据依赖关系,决定哪些目标要重新生成。
7. 执行生成命令

### 5.3 在规则中使用通配符

make与shell相同支持三种通配符号：

- `*`:通用匹配符号，真实的'*'可以使用`\*`来表示
- `?`:判断符号:
- `[...]`:
- `$@`:表示规则中的目标文件集。
    + 在模式规则中，如果有多个目标，那么，"$@"就是匹配于目标中模式定义的集合。
- `$%`:仅当目标是函数库文件中，表示规则中的目标成员名。
    + 例如，如果一个目标是"foo.a (bar.o)"，那么，"$%"就是"bar.o"，"$@"就是"foo.a"。如果目标不是函数库文件（Unix下是[.a]，Windows下是 [.lib]），那么，其值为空。
- `$<`:依赖目标中的第一个目标名字。
    + 如果依赖目标是以模式（即"%"）定义的，那么"$<"将是符合模式的一系列的文件集。注意，其是一个一个取出来的。
- `$?`:所有比目标新的依赖目标的集合。以空格分隔。
- `$^`:所有的依赖目标的集合。以空格分隔。
    + 如果在依赖目标中有多个重复的，那个这个变量会去除重复的依赖目标，只保留一份。
- `$+`:这个变量很像"$^"，也是所有依赖目标的集合。只是它不去除重复的依赖目标。
- `$*`:这个变量表示目标模式中"%"及其之前的部分。
    + 如果目标是"dir/a.foo.b"，并且目标的模式是"a.%.b"，那么，"$*"的值就是"dir /a.foo"。这个变量对于构造有关联的文件名是比较有较。如果目标中没有模式的定义，那么"$*"也就不能被推导出，但是，如果目标文件的后缀是 make所识别的，那么"$*"就是除了后缀的那一部分。例如：如果目标是"foo.c"，因为".c"是make所能识别的后缀名，所以，"$*"的值 就是"foo"。这个特性是GNU make的，很有可能不兼容于其它版本的make，所以，你应该尽量避免使用"$*"，除非是在隐含规则或是静态模式中。如果目标中的后缀是make所不 能识别的，那么"$*"就是空值。
例如：
```makefile

print:*.c
    lpr -p $?
    touch print
```

### 5.4文件查询

makefile中可以使用VPATH来指定文件查找目录。使用方法如下：

- 1.vpath <pattern> <directories>:为符合模式<pattern>的文件指定搜索目录<directories>。
- 2.vpath <pattern> ：清除符合模式<pattern>的文件的搜索目录。
- vpath:清除所有已经被设置好了的文件搜索目录

```makefile
#指定文件搜索在../headers和当前目录下，查找所有以`.h`结尾的文件

vpath %.h ../headers
#这里是先在foo中搜索，然后在blish,最后是bar

vpath %.c foo:bar
vpath % blish
```

### 5.5 伪目标

makefile中可以使用`.PHONY:xxx`的方式来设置伪目标，类似于shell中的`alias`操作别名
伪目标一般没有依赖文件，主要是相关操作。也可以使用依赖来化简操作。

```makefile
all:prog1 prog2 prog3
.PHONY:all

prog1:prog1.o utils.o
        cc -o prog1 prog1.o utils.o
prog2:...

#也可以使用伪目标作为依赖，实现操作如下
cleanall : cleanobj cleandiff
    rm program
cleanobj:
    rm *.o
cleandiff:
    rm *.diff

```
因为makefile会将第一个目标作为默认目标，因此可以使用伪目标依赖，来实现makefile的多目标依赖。

### 5.7 静态模式

使用静态模式可以更加容易地定义多目标的规则。使用语法如下

```makefile
<targets ...>: <target-pattern>: <prereq-patterns ...>
    <commands>
    ...

```
target:定义了一系列目标，可以有通配符，是目标的集合
target-pattern:指明了target的模式，也是目标集模式
prereq-patterns:是目标依赖模式对target-pattern的再次依赖定义

使用示例：

```makefile

    objects = foo.o bar.o
    all: $(objects)
$(objects): %.o: %.c
    $(CC) -c $(CFLAGS) $< -o $@

```
