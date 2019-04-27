---
layout:     post
title:      C++ Primer 学习笔记 (三)
subtitle:   C++ Primer 学习记录 (三)
date:       2019-4-25
author:     王鹏程
header-img: img/post-bg-ios10.jpg
catalog: true
tags:
    - C++
    - 基础编程
---

>2019-4-27 20:17:29 编写笔记如下

## 第五章 语句
没什么好写的。

## 第六章 函数基础

在函数中可以使用`static`来进行静态局部变量的全局作用域。使得局部变量的生存周期可以一直持续到程序结束。
注意当使用指针作为函数传入参数的时候，函数内部会拷贝传入参数指针，指针不同但是指向的地址和变量相同。当使用引用时，是传给函数使用对象的另外一个名字。因此在函数参数传递时应该，尽量使用引用参数。如下：

```c++
bool isShorter(const string &sl;const string &s2)
{
    return s1.size()<s2.size();
}
```
```c++
int  mian(int argc,char *argv[])
{

}

//第二个形参argv是一个数组，它的元素是指向c风格字符串的指针；因为第二个形参是数组，所以main函数也可以定义成：

int main(int argc, char const **argv) {
    /* code */
    return 0;
}


```


### initialzer_list 形参

当函数的实参数量未知；但是全部实参的类型相同，我们可以使用initializer_list类型的形参。详细描述如下表
 
<table>
    <thead>
        <tr>
            <th colspan="2">initializer_list 提供的操作</th>
        </tr>
        <tr>
            <th>操作</th>
            <th>解释</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>`initializer_list <T> lst `</td>
            <td>默认初始化；类型的空列表</td>
        </tr>
        <tr>
            <td>`initializer_list <T> lst {a,b,c...}; `</td>
            <td>lst的元素数量和初始值一样多；lst的元素是对应初始值的副本；列表中的元素是const</td>
        </tr>
        <tr>
            <td>` lst2(lst) | lst2=lst `</td>
            <td>拷贝复制一个元素</td>
        </tr>
        <tr>
            <td>` lst.size() `</td>
            <td align="center">列表中的元素数量</td>
        </tr>
        <tr>
            <td>` lst.begin() `</td>
            <td>返回指向lst中首元素的指针</td>
        </tr>
        <tr>
            <td>` lst.end() `</td>
            <td>返回指向lst中尾元素下一位置的指针</td>
        </tr>
    </tbody>
</table>


```c++

void error_msg(initializer_list<string> il)
{
    for(auto beg=il.begin();beg!=il.end();++beg)
    {
        cout<<*beg<<" ";//连续输出错误的函数信息
    }
    cout<<endl;

}

```

#### 省略符形参

为了方便c++程序访问某些特殊的c代码而设置的，这些大妈使用了名为`varargs`的c标准库功能。

```c++

void foo(parm_list,...);
void foo(...);

```
#### 值是如何被返回的
返回复制一个临时变量，该临时变量就是函数调用的结果。
注意：
1. 不要返回局部对象的引用或者指针，因为局部变量的引用和指针会随着局部变量的结束而终止，因此，返回的引用和指针会不在有效的内存区域内。
2. 函数的返回类型决定函数是否是左值，调用一个返回引用的函数得到左值，其它返回类型得到右值。


