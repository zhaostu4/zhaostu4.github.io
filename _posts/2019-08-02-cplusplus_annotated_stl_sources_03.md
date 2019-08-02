---
layout:     post
title:      STL 源码剖笔记(三)
subtitle:   STL 源码剖析笔记(三)
date:       2019-07-06
author:     王鹏程
header-img: img/post-bg-ios10.jpg
catalog: true
tags:
    - C++
    - 基础编程
    - STL源码解析
---

> 2019-08-02 16:09:52

## 算法

### 算法总览和分类

**STL算法总览**

![STL算法总览01](https://wangpengcheng.github.io/2019-08-02-16-13-21.png)

![STL算法总览02](https://wangpengcheng.github.io/2019-08-02-16-14-40.png)

![STL算法总览03](https://wangpengcheng.github.io/2019-08-02-16-18-06.png)

![STL算法总览04](https://wangpengcheng.github.io/2019-08-02-16-18-57.png)

**算法分类**

- 质变算法：会改变操作对象之值；如copy、swap、replace、fill、remove、permutation、aprtition、random shuffling、sort等
- 非质变算法：不改变操作对象之值：find、search、count、for_each、equal、mismatch、max、min等

**STL算法的一般形式**

质变算法一半提供两个版本：in-place--就地改变操作对象;copy--将操作对象的内容复制一份副本(通常以_copy结尾)，操作后返回。

### 6.3 数值算法<stl_numeric.h>

数值算法一般包含在表头<numeric>中，实现是在<stl_numeric.h>文件中

- accumulate 关键操作: init=init+*i(第一版本)、init=binary_op(init,*i*)(第二个版本)
- adjacent_difference:计算相邻元素的差额关键代码： *i-*(i-1)；
- inner_product:能够计算一般内积，`result=result+(*i)**(first2+(i-first))`
- partial_sum计算局部总和
- power:计算指数([C++中位运算的使用方法](https://blog.csdn.net/a1351937368/article/details/77746574/);[
STL源码分析之power算法](https://blog.csdn.net/u012062760/article/details/46401115);[快速计算x的n次幂](https://blog.csdn.net/MoreWindows/article/details/7174143))

```c++
T power(T x,Integer n,Monoidoperation op)
{
    if(n==0){
        return identity_element(op);
    }else{
        //进行位运算直到为偶数，这里主要是为了过滤掉低位的0

        while((n&1)==0){
            n>>=1;
            x=op(x,x);
        }
        //重新计算x，n变为偶数

        T result=x;
        n>>=1;
        //

        while(n!=0)
        {
            x=op(x,x);
            //发现是奇数;将奇数去除，即乘一个x

            if((n&1)!=0){
                result=op(result,x);
            }
            //除以2

            n>>=1;
        }
        return result;
    }
}
/*

第一步判断n是否为0；
第一个while循环，尽可能的将底数变的更大，更好进行后续的计算，经过第一个while之后，n肯定是一个奇数了，再进入第二个循环，如果是1，则直接返回。
第二个while循环感觉就是求当前底数（x的偶数次平方，y）的奇数（m）次平方的过程，也就是求m的二进制各个位是否为1的过程，如果当前位M为1，则结果额外乘以y的2^M次方~

*/

```

在这里顺道测试了一下GCC的stl的性能，发现结果比较感人啊，迷之编译器优化。。。。

```c++

#include<bits/stdc++.h>
#include<cmath>
using namespace std;
//朴素算法
long pusu(int x,int n)
{
    long num=1;
    for(int i=0;i<n;i++)
    {
        num*=x;
    }
    return num;
}
//分治算法----快速幂
long fenzhi(int x,int n)
{
    long num=1;
    while(n)
    {
        if((n&1)!=0)
        {
            num*=x;
            n--;
        }
        x*=x;
        n/=2;
    }
    return num;
}
//stl算法
long stl_pow(int x,int n)
{
    if (n == 0)
    {
        return 1;
    }
    else
    {
        while ((n & 1) == 0)
        {
            n >>= 1;
            x *= x;
        }
    }
    long result = x;
    n >>= 1;
    while (n != 0)
    {
        x *= x;
        if ((n & 1) != 0)
            result *= x;
        n >>= 1;
    }
    return result;
}
int main()
{
    int x=3,n=5,k=2;
    //cin>>x>>n;
   for(int i=0;i<k;++i){
    ++x; 
  for(int j=0;j<k;++j){
    ++n;
    long result;
    clock_t start_time=clock();
    long temp1=pusu(x,n);
    cout<<"朴素算法结果:"<<temp1<<endl;
    clock_t end_time=clock();
    printf("Running time is: %lfms\n",static_cast<double>(end_time-start_time)/CLOCKS_PER_SEC*1000);
    clock_t start_time1=clock();
    long temp2=fenzhi(x,n);
    cout<<"分治算法结果:"<<temp2<<endl;
    clock_t end_time1=clock();
    printf("Running time is: %lfms\n",static_cast<double>(end_time1-start_time1)/CLOCKS_PER_SEC*1000);
    clock_t start_time2=clock();
   cout<<"STL_power:"<<stl_pow(x,n)<<endl;    
   clock_t end_time2=clock();
   printf("Running time is: %lfms\n",static_cast<double>(end_time2-start_time2)/CLOCKS_PER_SEC*1000);
   clock_t start_time3=clock();
   cout<<"STL结果:"<<std::pow(x,n)<<endl; 
   clock_t end_time3=clock();
   printf("Running time is: %lfms\n",static_cast<double>(end_time3-start_time3)/CLOCKS_PER_SEC*1000);
   printf("x %d,n %d \n",x,n);
   printf("--------------------\n");
    }
}
return 0;
}
/* 结果

朴素算法结果:4096
Running time is: 0.097000ms
分治算法结果:4096
Running time is: 0.006000ms
STL_power:4096
Running time is: 0.009000ms
STL结果:4096
Running time is: 0.025000ms
x 4,n 6 
--------------------
朴素算法结果:16384
Running time is: 0.006000ms
分治算法结果:16384
Running time is: 0.005000ms
STL_power:16384
Running time is: 0.007000ms
STL结果:16384
Running time is: 0.009000ms
x 4,n 7 
--------------------
朴素算法结果:390625
Running time is: 0.005000ms
分治算法结果:390625
Running time is: 0.007000ms
STL_power:390625
Running time is: 0.007000ms
STL结果:390625
Running time is: 0.009000ms
x 5,n 8 
--------------------
朴素算法结果:1953125
Running time is: 0.006000ms
分治算法结果:1953125
Running time is: 0.007000ms
STL_power:1953125
Running time is: 0.006000ms
STL结果:1.95312e+06
Running time is: 0.009000ms
x 5,n 9 
--------------------

*/


```

### 6.4 基本算法

基本常用算法定义在<stl_algobase.h>中，其它算法定义于<stl_algo.h>

基本函数有: equal,fill,fill_n,iter_swap,lexicographical_compare,max,min,mismatch,swap;这些在之前的c++primer阅读笔记中有介绍，不再过多赘述。

lexicographical_compare：字典排列方式对两个序列，进行比较

- 如果第一序列的元素较小，返回true,否则返回false
- 如果达到last1而尚未达到last2,返回true
- 如果达到last2而尚未达到last1,返回false
- 同时达到，返回false

![copy算法的完整脉络](https://wangpengcheng.github.io/2019-08-02-21-23-34.png)

当复制位置的起点在复制区间内时，将会发生错误。为了防止上述错误的发生，一般都会先把目标区段使用memmove()复制下来，再进行操作。

![copy算法需要特别注意区间重叠的情况](https://wangpengcheng.github.io/2019-08-02-21-26-47.png)

copy改变的是[result,result+(last-first))中的迭代器所指对象，而并非更改迭代器本身。它会为输出区间的元素赋予新值，而不是产生新的元素。它不能改变输出区间的迭代器个数。因此不能将元素插入空容器中。

**copy_backward 反向拷贝**

![copy_backward操作示意图](https://wangpengcheng.github.io/2019-08-02-21-42-23.png)

### 6.5 set 相关算法

STL提供了四种set(集合)相关的算法，分别是并集(union),交集(intersection)，差集(difference)、对称集(symmetic difference)

set_union:求并集--set_union(first1,last1,first2,last2,ostream_iterator<int>(cout,""));

比较的关键代码：

```c++
template <class InputIterator1,class InputIterator2,class OutputIterator>
OutputIterator set_union(InputIterator1 first1,InputIterator1 last1,
                        InputIterator2 first2,InputIterator2 last2)
{
    while(first1!=last1&&first2!=last2) {
        //在两个区间之内分别移动迭代器，首先将元素较小者(假设为A区)记录于目标区域；然后移动A区迭代器使之前进；同时另一个迭代器不懂，然后进行新一次的比较大小、记录值、迭代器、迭代器移动，直到两区中有一个区达到尾端。如果元素相等，取S1者记录于目标区，并同时移动两个迭代器

        if(*first1<*first2){
            *result=*first1;
            ++first1;
        }else if(*first2<*first1){
            *result=*first2;
            ++first2;
        //*first2==*first1

        }else{
            *result=*first1;
            ++first1;
            ++first2;
        }
        ++result;
    }
}

```

![set_union分析](https://wangpengcheng.github.io/2019-08-02-21-52-12.png)

#### 6.5.2 set_intersection构造交集

基本上就是上面的条件判断反过来了，当相等时迭代器才增加。

![set_intersection步进分析](https://wangpengcheng.github.io/2019-08-02-22-08-15.png)

#### 6.5.3 set_difference

![set_difference步进分析](https://wangpengcheng.github.io/2019-08-02-22-12-26.png)

#### 6.5.4 set_symmetric_difference 构造对称差集

![set_symmetric_difference](https://wangpengcheng.github.io/2019-08-02-22-14-24.png)

### 6.7 其它算法



