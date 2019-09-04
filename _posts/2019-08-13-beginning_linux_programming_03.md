---
layout:     post
title:      Linux程序设计 学习笔记 (一)
subtitle:   Linux程序设计 学习笔记 (一)
date:       2019-09-03
author:     王鹏程
header-img: img/post-bg-ios10.jpg
catalog: true
tags:
    - C/C++
    - Linux
    - 操作系统
    - 程序设计
---

# Linux 程序设计 阅读笔记

## 参考链接：

- [Linux内核文档首页](https://www.kernel.org/doc/Documentation/)
- [Linux文档](https://linux.die.net/)
- [Linux c 开发手册](https://legacy.gitbook.com/book/wizardforcel/linux-c-api-ref/details)
- [Linux Kernel API](https://www.kernel.org/doc/htmldocs/kernel-api/index.html)
- [书中代码地址](http://www.wrox.com/WileyCDA/WroxTitle/Beginning-Linux-Programming-4th-Edition.productCd-0470147628,descCd-DOWNLOAD.html)

## 第6章 使用curses函数库管理基于文本的屏幕

### 6.1 使用curses函数库进行编译

添加头文件`-I/usr/include/nucurses`,添加动态链接库`-lncurses`。

curses中输出字符的过程如下：

- 使用curses函数刷新逻辑屏幕。
- 要求curses使用refresh函数来刷新物理屏幕。

逻辑屏幕的布局通过一个字符数组来实现，它以屏幕的左上角为坐标原点。一般坐标表示(行，列)，y值在前、x值(列号)在后。

![相关函数](https://wangpengcheng.github.io/img/2019-09-04-15-27-49.png)

一个简单示例：

```c++

#include <unistd.h>
#include <stdlib.h>
#include <curses.h>

int main()
{
    //初始化屏幕

    initscr();
    //移动画笔位置
    move(5,15);

    printw("%s","Hello word");
    //刷新屏幕

    refresh();
    sleep(2);
    //结束窗口

    endwin();
    exit(EXIT_SUCCESS);
}
```
### 6.3 屏幕

使用`WINDOW *initscr(void)`进行窗口的初始化。这个函数在一个程序中只能调用一次。当结束窗口时就是使用`endwin()`函数进行相关资源的销毁，当函数成功时返回OK否则返回ERR。

#### 6.3.1 输出到屏幕

curses函数库提供了一些用于刷新屏幕的基本函数，它们是：


- `int addch(const chtype char_to_add)`:。
- `int addchstr(chtype  *const string_to_add)`:。
- `int printw(char* format,...)`:
- `int refresh(void)`
- `int box(WINDOW *win_ptr,chtype vertical_char,chtype horizontal_char)`
- `int insch(chtype char_to_insert)`：插入一个字符，将已有字符向右移动
- `int insertln(void)`
- `int delch(void)`
- `int delectln(void)`
- `int beep(void)`
- `int flash(void)`
- `int erase(void)`:清除屏幕，在每个屏幕位置写上空白字符。
- `int clear(void)`:调用erase后，再使用clearok来强制重现屏幕原文。彻底清除整个屏幕。
- `int clrtobot`函数清除当前光标位置直到屏幕结尾的所有内容。
- `int clrtobot`函数清除当前光标位置直到光标所处行位。
- `int move(int new_y,int new_x)`:将逻辑光标的位置移动到指定地点。希望变化立即显现使用`refresh`函数。
- `int leaveok(WINDOW *window_ptr,bool leave_flag)`；添加bool标志位，控制在屏幕刷新后curses刷新物理光标所放的位置。默认false，刷新后，硬件光标停留在屏幕上逻辑光标所处的位置。true,硬件光标会被随机地防止在屏幕的任意位置之上。

注意：curses拥有自己的字符类型chtype，比标准char类型包含跟多的二进制。

#### 6.3.5 字符属性

- `int attron(chtype attribute)`：启用指定的属性
- `int attroff(chtype attribute)`：关闭指定的属性
- `int attrset(chtype attribute)`:设置curses属性。
- `int standout(void)`:标准的输出
- `int standend(void)`:标准输出

### 6.4 键盘

设置键盘的相关函数

- `int echo(void)`：输出。
- `int noecho(void)`：。
- `int cbreak(void)`:
- `int raw(void)`:
- `int noraw(void)`:

#### 6.4.2 键盘输入

`int getch(void)`、`int getstr(char *string)`、`int getnstr(char* string,int number_of_characters)`、`int scanw(char *format,...)`。

### 6.5 窗口

#### 6.5.1 WINDOW窗口

使用`WINDOW * newwin(int num_of_lines,int num_of_cols,int start_y,int start_x)`来出那个键一个新窗口。`int delwin(WINDOW *window_to_delete)`删除一个窗口

注意：千万不要尝试删除curses自己的窗口和stdscr和curscr。

#### 6.5.3 移动和更新窗口

```c++
#include <curses.h>
int mvwin(WINDOW *window_to_move,int new_y,int new_x);
int wrefresh(WINDOW *window_ptr);
int wclear(WINDOW *window_ptr)
int werase(WINDOW *window_ptr);
int touchwin(WINDOW *window_ptr);
int scrollok(WINDOW *window_ptr,bool scroll_flag);
int scroll(WINDOW *window_ptr);
```

使用示例：

```c++
/*  As usual let's get our definitions sorted first.  */

#include <unistd.h>
#include <stdlib.h>
#include <curses.h>

int main()
{
    WINDOW *new_window_ptr;
    WINDOW *popup_window_ptr;
    int x_loop;
    int y_loop;
    char a_letter = 'a';
    //初始化显示

    initscr();

/*  Then we fill the base window with characters,
    refreshing the actual screen once the logical screen has been filled:

    move(5, 5);
    printw("%s", "Testing multiple windows");
    refresh();

    for (x_loop = 0; x_loop < COLS - 1; x_loop++) {
        for (y_loop = 0; y_loop < LINES - 1; y_loop++) {
            mvwaddch(stdscr, y_loop, x_loop, a_letter);
            a_letter++;
            if (a_letter > 'z') a_letter = 'a';
        }
    }

    refresh();
    sleep(2);
*/
/*  Now we create a new 10x20 window
    and add some text to it before drawing it on the screen.  */

    //创建新窗口

    new_window_ptr = newwin(10, 20, 5, 5);
    //移动窗口并进行写操作
    mvwprintw(new_window_ptr, 2, 2, "%s", "Hello World");

    mvwprintw(new_window_ptr, 5, 2, "%s", "Notice how very long lines wrap inside the window");
    //刷新窗口

    wrefresh(new_window_ptr);
    sleep(2);

/*  We now change the contents of the background window and, when we
refresh the screen, the window pointed to by new_window_ptr is obscured.  */

   a_letter = '0';
     for (x_loop = 0; x_loop < COLS - 1; x_loop++) {
        for (y_loop = 0; y_loop < LINES -1; y_loop++) {
            mvwaddch(stdscr, y_loop, x_loop, a_letter);
            a_letter++;
            if (a_letter > '9') a_letter = '0';
        }
    }

    refresh();
    sleep(2);

/*  If we make a call to refresh the new window, nothing will change,
    because we haven't changed the new window.  */

    wrefresh(new_window_ptr);
    sleep(2);

/*  But if we touch the window first
    and trick curses into thinking that the window has been changed.
    The next call to wrefresh will bring the new window to the front again.  */

    touchwin(new_window_ptr);
    wrefresh(new_window_ptr);
    sleep(2);

/*  Next, we add another overlapping window with a box around it.  */

    popup_window_ptr = newwin(10, 20, 8, 8);
    box(popup_window_ptr, '|', '-');
    mvwprintw(popup_window_ptr, 5, 2, "%s", "Pop Up Window!");
    wrefresh(popup_window_ptr);
    sleep(2);

/*  Then we fiddle with the new and pop-up windows before clearing and deleting them.  */

    touchwin(new_window_ptr);
    wrefresh(new_window_ptr);
    sleep(2);

    wclear(new_window_ptr);
    wrefresh(new_window_ptr);
    sleep(2);
    
    delwin(new_window_ptr);

    touchwin(popup_window_ptr);
    wrefresh(popup_window_ptr);
    sleep(2);
    
    delwin(popup_window_ptr);
    
    touchwin(stdscr);
    refresh();
    sleep(2);

    endwin();
    exit(EXIT_SUCCESS);
}

```

#### 6.5.4 优化屏幕刷新

- `int wnoutrefresh(WINDOW *window_ptr)`：把那些字符发送到屏幕上，实际的发送工作由dpupdate完成
- `int doupdate(void)`：可以进行相关数据的更新工作。
- `WINDOW *subwin(WINDOW *parent,int num_of_lines,int num_of_cols,int start_y,int start_x)`：创建子窗口

#### 6.4 keypad模式

使用`int keypad(WINDOW *window_ptr,bool keypad_on)`设置keypad_on为true来启用keypad模式。让curses接管按键转义序列的处理工作。读取用户按下的键，还将返回与逻辑按键对应的KEY_定义。

注意：识别escape转义序列的过程是与时间相关的，为了能够区分单独按下Escape按键和一个以Escape字符开头的键盘转义序列。不能处理二义性的escape转义序列。如果终端上两个而不同的按键会产生完全相同的专业序列，curses将不会处理这个转义序列，因为它不知道该返回哪个逻辑按键。

下面是简单的使用示例：

```c
#include <unistd.h>
#include <stdlib.h>
#include <curses.h>

#define LOCAL_ESCAPE_KEY    27

int main() 
{
    int key;

    initscr();
    crmode();
    keypad(stdscr, TRUE);

/*  Next, we must turn echo off
    to prevent the cursor being moved when some cursor keys are pressed.
    The screen is cleared and some text displayed.
    The program waits for each key stroke
    and, unless it's q, or produces an error, the key is printed.
    If the key strokes match one of the terminal's keypad sequences, 
    then that sequence is printed instead.  */
    //截断输出

    noecho();
    //清除

    clear();
    //移动输出

    mvprintw(5, 5, "Key pad demonstration. Press 'q' to quit");
    move(7, 5);
    //刷新

    refresh();
    //获取字符

    key = getch();
    while(key != ERR && key != 'q') {
        move(7, 5);
        clrtoeol();

        if ((key >= 'A' && key <= 'Z') ||
            (key >= 'a' && key <= 'z')) {
            printw("Key was %c", (char)key);
        }else {
            switch(key) {
            case LOCAL_ESCAPE_KEY: printw("%s", "Escape key"); break;
            case KEY_END: printw("%s", "END key"); break;
            case KEY_BEG: printw("%s", "BEGINNING key"); break;
            case KEY_RIGHT: printw("%s", "RIGHT key"); break;
            case KEY_LEFT: printw("%s", "LEFT key"); break;
            case KEY_UP: printw("%s", "UP key"); break;
            case KEY_DOWN: printw("%s", "DOWN key"); break;
            default: printw("Unmatched - %d", key); break;
            } /* switch */
        } /* else */

        refresh();
        key = getch();
    } /* end while */

    endwin();
    exit(EXIT_SUCCESS);
}

```

### 6.8 彩色显示

使用has_color()和start_color()可以实现对颜色例程的初始化。

```c++
#include <curses.h>
bool has_colors(void);
int start_color(void);

```
在将颜色作为属性使用之前，你必须首先调用`init_pair()`函数对准备使用的颜色组合进行初始化。对颜色属性的访问是通过COLOR_PAIR函数来完成的。

```c
#include <curses.h>

//定义颜色组合

int init_pair(short pair_number.short foreground,short background);
//

int COLOR_PAIT(int pair_number);
//获取已有的颜色组合

int pair_content(short pair_number,short *foreground,short *background);
//重定义色彩

int init_color(short color_number,short red,short green,short blue); 
```

颜色的调用示例：

```c

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <curses.h>

int main()
{
    int i;

    //初始化curses

    initscr();
    //检查是支持色彩的终端

    if (!has_colors()) {
        endwin();
        fprintf(stderr, "Error - no color support on this terminal\n");
        exit(1);
    }
    //初始化色彩选项

    if (start_color() != OK) {
        endwin();
        fprintf(stderr, "Error - could not initialize colors\n");
        exit(2);
    }

/*  We can now print out the allowed number of colors and color pairs.
    We create seven color pairs and display them one at a time.  */
    //清除屏幕

    clear();
    //输出文字

    mvprintw(5, 5, "There are %d COLORS, and %d COLOR_PAIRS available", 
             COLORS, COLOR_PAIRS);
    //进行刷新

    refresh();
    //初始化颜色

    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_GREEN);
    init_pair(3, COLOR_GREEN, COLOR_RED);
    init_pair(4, COLOR_YELLOW, COLOR_BLUE);
    init_pair(5, COLOR_BLACK, COLOR_WHITE);
    init_pair(6, COLOR_MAGENTA, COLOR_BLUE);
    init_pair(7, COLOR_CYAN, COLOR_WHITE);
    //输出显色字符串

    for (i = 1; i <= 7; i++) {
        //开启属性设置

        attroff(A_BOLD);
        //设置颜色属性

        attrset(COLOR_PAIR(i));
        //输出文字

        mvprintw(5 + i, 5, "Color pair %d", i);
        //设置属性

        attrset(COLOR_PAIR(i) | A_BOLD);
        //输出文字

        mvprintw(5 + i, 25, "Bold color pair %d", i);
        //刷新

        refresh();
        //睡眠

        sleep(1);
    }

    endwin();
    exit(EXIT_SUCCESS);
}

```

### 6.9 pad
curses提供了特殊的数据结构pad来控制尺寸大于正常窗口的逻辑屏幕。相关接口如下;

```c
//初始化pad

WINDOW *newpad(int number_of_lines,int number_of_columns);
//执行刷新操作。指定放到屏幕上的pad范围和放置在屏幕上的位置。prefresh函数用于完成这一功能。


int prefresh(WINDOW *pad_ptr,
            int pad_row,
            int pad_column,
            int screen_row_min,//显示区域的坐标范文
            int screen_col_min,
            int screen_row_max,
            int screen_col_max
            )
```
pad的简单示例：

```c
#include <unistd.h>
#include <stdlib.h>
#include <curses.h>

int main() 
{
    WINDOW *pad_ptr;
    int x, y;
    int pad_lines;
    int pad_cols;
    char disp_char;

    initscr();

    pad_lines = LINES + 50;
    pad_cols = COLS + 50;
    //创建pad

    pad_ptr = newpad(pad_lines, pad_cols);

    disp_char = 'a';
    //在内部添加字符串

    for (x = 0; x < pad_lines; x++) {
        for (y = 0; y < pad_cols; y++) {
            mvwaddch(pad_ptr, x, y, disp_char);
            if (disp_char == 'z') disp_char = 'a';
            else disp_char++;
        }
    }

/*  We can now draw different areas of the pad on the screen at different locations before quitting.  */
    //更新局部窗口
    
    prefresh(pad_ptr, 5, 7, 2, 2, 9, 9);
    sleep(1);
    //更新大范围窗口

    prefresh(pad_ptr, LINES + 5, COLS + 7, 5, 5, 21, 19);
    sleep(1);

    delwin(pad_ptr);

    endwin();
    exit(EXIT_SUCCESS);
}

```

## 第七章 数据管理

