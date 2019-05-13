---
layout:     post
title:      C++ Primer 学习笔记 (六)
subtitle:   C++ Primer 学习记录 (六)
date:       2019-5-12
author:     王鹏程
header-img: img/post-bg-miui6.jpg
catalog: true
tags:
    - C++
    - 基础编程
    - std 标准库
---

>>2019-5-12 :19:44:52

## 泛型算法

泛型算法是指使用迭代器为媒介的通用型算法；泛型算法本身不会执行容器的操作，他们只会运行于迭代器智商，执行迭代器的操作--算法永远不会改变底层容器的大小。算法可能改变容器中保存的元素值。

**`accumulate` 求和**

```c++

int sum =accumulate(vec.cbegin(),vec.cend(),0);//对元素求和初值是0

string sum=accumulate(v.cbegin(),v.cend(),string(''));//字符串求和

equal(roster1.cbegin(),roster1.cend(),rosterl2.cbegin());//比较元素，数量相同且一样多的时候才返回`true`

fill(vec.begin(),vec.begin()+vec.size()/2,10);//将容器的一个子序列设置为10

back_inserter();//插入元素，检查是否存在

copy(begin(a1),end(a1),a2);//把a1的内容拷贝给a2

replace(ilist.begin(),ilist.end(),0,42);//查找所有元素为0的值，并将它更改为42

replace_copy(ilis.cbegin(),ilist.cend(),back_inserter(ivec),0,42);//ilist指出调整序列之后的保存位置。

sort(word.begin(),word.end());//按照字典排序

auto end_unique=unique(words.begin(),words.end());//除去重复单词

``` 

注意：
 
- 算法不检查写操作
- 向目的位置迭代器写入数据的算法，假定目的位置足够大，能容纳要写入的元素。

### 定制操作
许多算法的默认关键操作符，并不能满足我们的需求，因此，需要我们来重载`sort`的默认行为。

#### 向算法传递函数
sort可以接受第三个参数，即谓词--一个可用的表达式，其返回结果是一个能用作条件的值。标准库中分为
**一元谓词**(只接受单一参数)和**二元谓词**(他们有两个参数)。例如下面的代码，重载了sort函数的基础比较操作

```c++
//排序函数

bool isShorter(const string &s1,const string &s2){
    return s1.size()<s2.size();
}
//按照长度由短到长排序
sort(words.begin(),words.end(),isShorter);

elimDups(words);//将words按照字典重新排序，并消除重复单词

stable_sort(words.begin(),words.end(),isShorter)；//按照长度重新排序，长度相同的单词维持字典序

//无需拷贝字符串

for (const auto &s :words)
{
    cout<<s<<"";//打印每个元素，以空格分割

}
cout<<endl;

```
### lambda 表达式

_参考链接：_ [c++11 lambda 表达式](https://www.cnblogs.com/DswCnblog/p/5629165.html);[c++ lambda表达式](https://www.cnblogs.com/jimodetiantang/p/9016826.html);

**lambda表达式的声明**

`[capture list] (params list) mutable exception-> return type { function body }`
各项含义如下：

1. `capture list`: 捕获外部变量列表
2. `params list`: 形参列表
3. `mutable`:用来说明是否可以修改捕获的变量
4. `exception`:异常设定
