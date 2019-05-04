---
layout:     post
title:      C++ Primer 学习笔记 (四)
subtitle:   C++ Primer 学习记录 (四)
date:       2019-5-4
author:     王鹏程
header-img: img/post-bg-miui6.jpg
catalog: true
tags:
    - C++
    - 基础编程
---
> 2019-5-4: 16:03

## 类

### 定义抽象数据
c++中类的实质就是数据的抽象实现和封装。抽象数据依赖于接口和实现，分离的编程技术；**定义在函数内部的函数是隐式的inline函数**

#### 类中的this关键字
_参考链接：_ [C++类中this指针的理解](https://www.cnblogs.com/liushui-sky/p/5802981.html); [C++ this 指针](https://www.runoob.com/cplusplus/cpp-this-pointer.html);[关于this指针](https://blog.csdn.net/shidantong/article/details/80104710);

首先声明`this`指针是c++为了方便管理类中的函数而产生的，每个对象的`this`指针都指向对象本身，可以使用对象的非静态方法。`this`指针是编译器给予的每个成员函数的隐含参数，用来指向调用对象。
注意：

1. 友元函数没有 this 指针，因为友元不是类的成员。只有成员函数才有 this 指针。
2. this指针其本身的内容是不能被改变的，其类型为：**类类型 * const**
3. this指针不是对象本身的一部分，不影响该对象的大小
4. this指针的作用域在类的 **非静态成员函数**的内部，只能在其内部进行使用，其他任何函数都不能，静态成员函数内部无this指针，后面会详述。
5.  this指针是类中非静态成员函数的第一个默认隐含参数，编译器自动传递和维护，用户不可显示传递
6.  this指针可以为空但是类内部的成员变量数据不能为空，因此当调用使用成员变量时，请确保成员变量不为空。

函数调用约定如下：

![函数调用约定表](https://img-blog.csdn.net/20180426163408707?watermark/2/text/aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3NoaWRhbnRvbmc=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70)

_thiscall关键调用：

- 它只能用在类的成员函数上
- 参数从右向左进行压栈
- 若参数个数确定，this指针通过ecx寄存器传递给被调用者；若参数不确定，this指针在所有参数被压栈后压入堆栈
- 对于参数不确定的函数，调用者清理堆栈，否则函数自己清理堆栈。

##### 构造函数
构造函数主要是c++中对于类对象的实例化的函数,这里需要强调一下c++中`new A`、`new A()`与`new A[]`的区别；

_参考链接：_ [C++中new 一个对象的时候加括号和不加括号的区别](https://blog.csdn.net/ywending/article/details/51096547);[C++ new A 和 new A() 的区别详解以及引申的各种初始化类型](https://blog.csdn.net/qq_34228570/article/details/79549958);

先说结论：

-  加括号调用没有参数的构造函数，不加括号调用默认构造函数或唯一的构造函数，看需求。
-  **C++在new时的初始化的规律可能为：对于有构造函数的类，不论有没有括号，都用构造函数进行初始化；如果没有构造函数，则不加括号的new只分配内存空间，不进行内存的初始化，而加了括号的new会在分配内存的同时初始化为0。**

POD(Plain old data):它是一个struct或者类，且不包含构造函数、析构函数以及虚函数。POD class没有用户定义的析构函数、拷贝构造函数和非静态的非POD类型的数据成员。而且，POD class必须是一个aggregate，没有用户定义的构造函数，没有私有的或者保护的非静态数据，没有基类或虚函数。它只是一些字段值的集合，没有使用任何封装以及多态特性。

对于`[new] T [object] {};`初始化一般有三种处理方式：

- 如果T有用户定义的缺省构造函数，直接调用；
- 如果T有编译器生成的缺省构造函数，先0值初始化再调用；
- 如果T根本不是类，直接0值初始化。

对于`[new] T object;`如果T是非class类型，则给出非确定值（不赋值），比如：`int i; double d; bool b;`；
0值初始化也单独作用于静态(或者线程局部）变量：`static T object;`

五种初始化类型：

- list initialization (since C++11) 
- aggregate initialization 这是list initialization对aggregate类型的特例 
- value initialization 值初始化 
- default initialization 缺省初始化 
- zero initialization 0值初始化

示例代码：

```c++
#include <iostream>  

using namespace std;

class A { public:int m; }; // POD  

class B { public:~B() {}; int m; }; // non-POD, compiler generated default ctor  

class C { public:C() :m() {}; ~C() {}; int m; }; // non-POD, list-initialising m

class D { public:D() {}; ~D() {}; int m; };  // non-POD，default-initialising m

int main()
{
    A *aObj1 = new A;
    A *aObj2 = new A();
    cout << aObj1->m << endl;
    cout << aObj2->m << endl;

    B *bObj1 = new B;
    B *bObj2 = new B();
    cout << bObj1->m << endl;
    cout << bObj2->m << endl;

    C *cObj1 = new C;
    C *cObj2 = new C();
    cout << cObj1->m << endl;
    cout << cObj2->m << endl;

    D *dObj1 = new D;
    D *dObj2 = new D();
    cout << dObj1->m << endl;
    cout << dObj2->m << endl;

    delete aObj1;
    delete aObj2;
    delete bObj1;
    delete bObj2;
    delete cObj1;
    delete cObj2;

    getchar();
    return 0;
}
/*result
-842150451
0
-842150451
0
0
0
-842150451
-842150451
注意不同编译器可能产生不同的结果，GCC中结果：
0
0
38158368
0
0
0
38158400
0
结论（以GCC为准）：
new A：0

new A()：0

new B：不确定的值

new B()：0

new C：0

new C()：0

new D：不确定的值

new D()：0

*/


```

注意：
1. 只有当类没有声明任何构造函数时，编译器才会自动地生成默认构造函数。
2. 可以在构造函数之后添加`=default`来要求编译器生成默认构造函数。

### 访问控制与封装
访问说明符`public`、`private`、`protect`来加强类的封装性。注意`struct`关键字中定义第一个访问说明符之前的成员是`public`，如果使用`class`关键字，成员是`private`的。因此当希望所有的类成员是`public`时使用`struct`；希望是`private`时使用`class`。

**友元(friend)**:类允许其他类或者函数访问它的非公有成员，将函数成为友元只需要增加一条`friend`关键字就可以了。注意友元函数，必须在友元声明之外再专门对函数进行一次声明。

定义类型的成员必须先定义后使用例如：

```c++
class Screen
{
public:
    //使用类型别名等价地声明一个类型名字

    typedef std::string::size_type pos;
    Screen();
    ~Screen();
private:
    pos cursor=0;
    pos height=0,width=0;
    std::string contents;
    
};
```
类的友元使用示例如下

```c++
//screen.cpp

class Screen
{
public:
    //使用类型别名等价地声明一个类型名字

    typedef std::string::size_type pos;
    Screen();
    ~Screen();
    //定义友元

    //Windows类可以访问Screen类的私有部分

    friend class Windows;
    //成员函数作为友元

    friend void Windows::clear(ScreenIndex);

private:
    pos cursor=0;
    pos height=0,width=0;
    std::string contents;
    
}

//windows.cpp

class Windows
{
public:
    //窗口中每个屏幕的编号

    using ScreenIndex=std::vector<Screen>::size_type;
    //按照编号将指定的Screen重置为空白
private:
    
    std::vector<Screen> screen{screen(24,80,'')};

public:
    void clear(ScreenIndex i)
    {
        //s是一个Screen的引用，指向我们想清空的那个平米

        Screen &s=screen[i];

        //将那个选定的Screen重置为空白

        s.contents=string(s.height*s.width,'');
    }
}

```
注意：

1. 友元的函数未声明直接出现时，我们隐式的假定该名字在当前作用域中是可见的，要想正常使用，该函数必须先被定义过。


**`mutable`**关键字： 类成员变量使用`mutable`关键字即便是`const`对象也能更改类的成员变量。

类的成员函数后添加`const`关键字，表示不会修改类中的成员变量；并且有一下几点规则：

1. 在类中被const声明的成员函数只能访问const成员函数，而非const函数可以访问任意的成员函数，包括const成员函数
2. 在类中被const声明的成员函数不可以修改对象的数据，不管对象是否具有const性质，它在编译时，以是否修改成员数据为依据，进行检查。
3. 加上mutable修饰符的数据成员，对于任何情况下通过任何手段都可修改，自然此时的const成员函数是可以修改它的

**向前声明：** 仅仅声明类而不去定义它。对于类而言，当类的名字出现过后，它就被认为是声明过了（但是尚未定义），因此类允许包含指向它自身类型的引用或者指针。例如：

```c++
class Link_screen
{
    Screen windows;
    Link_screen *next;
    Link_screen *prev;
}

```



