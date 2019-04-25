---
layout:     post
title:      C++ Primer 学习笔记 (二)
subtitle:   C++ Primer 学习记录 (二)
date:       2019-4-25
author:     王鹏程
header-img: img/post-bg-ios10.jpg
catalog: true
tags:
    - C++
    - 基础编程
---

>2019-4-25 12:31:29 编写笔记如下

## 第三章 字符串、向量和数组

### 使用using name space
c++中使用`using name space ` 来进行命名空间的选择，但是一般不建议直接在声明中使用。建议使用`spacename:: function`的格式；例如`std::cout`;并且头文件中，不应该包括`using`声明。

### 标准库类型 string
下面展示string的常用初始化方法：

```c++
std::string s1; //默认初始化，s1市一个空字符串
std::string s2=s1; //s2是s1的副本
std::string s3="hiya";//s3市该字符串字面值的副本
std::string s5("hiya");//s3是字面值“value”的副本，除了字面值最后的那个空字符除外
std::string s4(10,'c'); //s4 的内容是cccccccccc
```
`os<<s`、`is>>s`；从字符串的输入，输出流。例如：

```c++
#include <iostream>
using namespace std;
int main(int argc, char const *argv[])
{
    string s;
    cin>>s; //将string 对象读入s,遇到空白停止
    cout<<s<<endl;//输出
    return 0;
}
```
`getline()`:读取函数整行，直到遇到换行符为止。
注意`string 对象的size()`函数返回值类型是`string::size_type`;当他与一个具有负号的n比较时，肯定为true，因此建议使用`auto`来进行返回值的定义。防止`int`和`unsigned`可能带来的问题。

#### 标准库string重点字符处理--<cctype>头文件

|函数名称|功能|
|:---:|:---:|
|`isalnum(c)`|当c是字母或数字时为真|
|`isalpha(c)`|当c是字母时为真|
|`iscntrl(c)`|当c是控制字符时为真|
|`isdigit(c)`|当c是数字时为真|
|`isgraph(c)`|当c不是空格但可以打印时为真|
|`isalower(c)`|当c是小写字母时为真|
|`isprint(c)`|当c是可打印字符时为真|
|`ispunct(c)`|当c是标点符号时为真|
|`isspace(c)`|当c是空白时为真（空格，制表符、回车符、换行符、进纸符中的一种）|
|`issupper(c)`|当c是大写字母时为真|
|`isxdigit(c)`|当c是十六进制数字时为真|
|`tolower(c)`|转换字母为小写|
|`toupper(c)`|转换字母为大写|

使用示例：

```c++
string s("hello word !!!");
decltype(s.size()) punct_cnt=0;
for (auto c : s)//注意这里auto是拷贝无法改变其中的char的值，可以使用&c进行值的改变。
{
    if(ispunct(c))
    {
        ++punct_cnt;//标点负号计数值加1；
    }
    cout<<punct_cnt
        <<"punctuation characters in"<<s<<endl;
}
//输出结果
3 punctuation characters in hello word!!!
```
将输入十进制数字转化为十六进制：

```c++
#include <iostream>
#include <stack>
#include <sstream>
#include <string>
using namespace std;

int main(int argc, char const *argv[]) {
    const string test="0123456789ABCDEF";
    cout<<"please input number between 0 and 15"<<endl;
    string::size_type n, temp;//用于保存从输入流读取的数
    while((cin.peek()!=EOF)&&(cin.peek()!='\n'))
    {
        cin>>n;
        string result;//用于保存十六进制的字符串
       //else if(n>test.size()){
            for(int i=n;i>0;){
                temp=i%16;
                auto s1 = test[temp];//转化为十六进制
                result=s1+result;
                i=i>>4;
            }
        printf("%X\n",n);
        std::cout<<"this result is :"<<result<<std::endl;
        //result.clear();
    }
    printf("hello word");
    return 0;
}
```
### vector 向量介绍
#### 迭代器(iterator)介绍
迭代器类似于指针但是不同于指针；利用地址进行一次间接的迭代访问。
标准迭代器的运算符：
|运算符|功能|
|:---:|:---:|
|*iter|返回迭代器iter所指元素的引用|
|iter->mem|解引用iter并获取该元素的名为mem的成员，等价于(*iter).mem|
|++iter|令iter指示容器的下一个元素|
|--iter|令iter指示容器的上一个元素|
|iter1==iter2|判断是否指向同一个元素|
|iter1!=iter2|判断是否指向同一个元素|

```c++
string s("some string ");
if(s.begin()!=s.end())
{
    auto it=s.begin();//it表示s的第一个字符
    *it=toupper(*it);//当前字符改写成大写格式
}
```
注意c++中定义了箭头运算符：(`->`)把解引用和成员访问两个操作结合在一起，`it->mem`和`(*it).mem`表达的意思相同。



