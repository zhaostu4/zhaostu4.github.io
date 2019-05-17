---
layout:     post
title:      C++ Primer 学习笔记(六)
subtitle:   C++ Primer 学习记录(六)
date:       2019-05-12
author:     王鹏程
header-img: img/post-bg-ios10.jpg
catalog: true
tags:
    - C++
    - 基础编程
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
5. `return type`:返回类型
6. `function body`:函数体

省略的常见表达式有：

|序号|格式|
|:---|:---|
|1|`[capture list] (params list) -> return type {function body}`|
|2|`[capture list] (params list) {function body}`|
|3|`[capture list] {function body}`|

其中：

- 格式1声明了const类型的表达式，这种类型的表达式不能修改捕获列表中的值。
- 格式2省略了返回值类型，但编译器可以根据以下规则推断出Lambda表达式的返回类型： （1）：如果function body中存在return语句，则该Lambda表达式的返回类型由return语句的返回类型确定； （2）：如果function body中没有return语句，则返回值为void类型。
- 格式3中省略了参数列表，类似普通函数中的无参函数。

```c++
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

bool cmp(int a, int b)
{
    return  a < b;
}

int main()
{
    vector<int> myvec{ 3, 2, 5, 7, 3, 2 };
    vector<int> lbvec(myvec);

    sort(myvec.begin(), myvec.end(), cmp); // 旧式做法

    cout << "predicate function:" << endl;
    for (int it : myvec)
        cout << it << ' ';
    cout << endl;

    sort(lbvec.begin(), lbvec.end(), [](int a, int b) -> bool { return a < b; });   // Lambda表达式
    cout << "lambda expression:" << endl;
    for (int it : lbvec)
        cout << it << ' ';
}

```
lambde表达式的外部捕获

Lambda表达式通过在最前面的方括号[]来明确指明其内部可以访问的外部变量，这一过程也称过Lambda表达式“捕获”了外部变量。

值捕获方式：

|捕获形式  |  说明|
|:---|:---|
|`[]`| 不捕获任何外部变量|
|`[变量名, …]`    |默认以值得形式捕获指定的多个外部变量（用逗号分隔），如果引用捕获，需要显示声明（使用&说明符）|
|`[this]` | 以值的形式捕获this指针|
|`[=]`| 以值的形式捕获所有外部变量|
|`[&]`| 以引用形式捕获所有外部变量|
|`[=, &x]`| 变量x以引用形式捕获，其余变量以传值形式捕获|
|`[&, x] `| 变量x以值的形式捕获，其余变量以引用形式捕获|

