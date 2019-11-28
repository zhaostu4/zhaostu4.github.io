---
layout:     post
title:      Unix网络编程-卷1
subtitle:   Unix网络编程-卷1-高级
date:       2019/11/28
author:     zhaostu4
header-img: img/post-bg-ios10.jpg
catalog: true
tags:
    - Unix
	- Unix网络编程
	- Unix网络编程-卷1
	- Unix网络编程-卷1-高级
    - 学习笔记
---

>> T:2019/11/28  W:四 17:0:11

[HTML]:@[TOC](《Unix网络编程》卷1 高级)

# 第12章 IPv4和IPv6的互操作性
## 12.1 概述
> 未来，因特网会逐渐从`IPv4`转换到`IPv6`所以**应用程序能够对两种协议协同工作**是很重要的，我们将讨论`IPv4应用`如何与`IPv6`进行通信
## 12.2 IPv4 客户与 IPv6 服务器
- 双栈主机一个很重要的特性就是技能处理`IPv4`又能处理`IPv6`客户。这是通过使用`IPv4`映射的`IPv6`地址来实现的
- 主机使用`IPv6`服务器，收到`IPv4`的帧时，会将`源IPv4地址`转换为一个映射的`IPv6地址`
	- `IPv4 地址`可以转换为一个唯一的`IPv6 地址`(`IPv4地址`补齐到`IPv6地址`格式, 方式是在前补`::ffff`)
	- 然后把数据报相应的字段给替换掉(包括`IP版本标识`字段)
---
- 补充 [IPv6 地址格式](https://www.ibm.com/support/knowledgecenter/zh/ssw_ibm_i_71/rzai2/rzai2ipv6addrformat.htm)
- `IPv6`中的双冒号:
	- 通过使用双冒号`(::)`替换一系列零来指定 `IPv6 地址`, 一个`IP 地址`中只可使用一次双冒号。
		- 例如: `IPv6` 地址: `ff06:0:0:0:0:0:0:c3` 可写作 `ff06::c3`。
- `IPv4`映射的`IPv6`地址使用此替代格式。
	- 此类型的地址用于将`IPv4`节点表示为`IPv6`地址。
		- 例如: `IPv4`地址`192.1.56.10`映射为`IPv6`地址`0:0:0:0:0:ffff:192.1.56.10`和 `::ffff:192.1.56.10/96`(短格式)
- **注意`IPv6地址`不能直接转`IPv4地址`**
---
- 一个`IPv4客户端`和`IPv6服务端`通信地步骤如下：
	- `IPv6服务器`启动后创建一个`IPv6监听套接字`
	- `IPv4客户`调用`getsockname`找到服务器地`A记录`。服务器既`有A也有AAAA记录`，因为它是**双栈**的
	- **客户调**用`connect`，发送一个`IPv4的SYN`给服务器
	- **服务器**收到这个`SYN`，把它标志为`IPv4`映射为`IPv6`，响应一个`IPv4 SYN/ACK`。连接建立后，	 由`accept`返回给服务器的地址就是这个`IPv4`映射的`IPv6地址`
	- 当**服务器向这个客户端发送数据**时，会使用客户端的`IPv4地址`，所以通信使用的全部都是`IPv4连接`
	- 如果服务器不检查这个地址是`IPv6`还是`IPv4`映射过来，它永远不会知道客户端的 `IP 类型`，客户端也不需要知道服务器的类型
- 大多数双栈主机遵循以下规则：
	- `IPv4 监听套接字`只能接受来自`IPv4 客户`的外来连接
	- 如果服务器有一个绑定了`IPv6 的监听套接字`，该套接字没设置 `IPV6_V6ONLY` 套接字选项，它**可以接收** `IPv4 连接`
	- 如果服务器有一个 `IPv6 监听套接字`，绑定了通配地址，该套接字设置了 `IPV6_V6ONLY` 套接字选项，它**只能接收** `IPv6 连接`
![TCP IPv4 客户与 IPv6 服务器](https://img-blog.csdnimg.cn/20191027215231177.png#pic_center)
- `IPv6` `UDP`服务器的情况与之类是, 差别在于数据报的地址格式有所不同.例如`IPv6服务器`收到来自某个`IPv4客户`的数据报,`recvfrom`返回的地址将是该客户端的`IPv6地址`(由于`IPv4`映射而来)

## 12.3 `IPv6`客户与`IPv4`服务器
- 客户机运行在双栈主机上并使用`IPv6套接字描述符`
- 过程:
	- 一个`IPv4 服务器`在只支持`IPv4`的主机上启动一个`IPv4 监听套接字`
	- `IPv6 客户`启动后调用`getaddrinfo`单纯查找`IPv6`的地址，因为服务器只支持`IPv4`，所以返回给客户端的是一个`IPv4`映射的`IPv6地址`
	- `IPv6 客户`在作为函数参数的 `IPv6 套接字地址结构`中设置这个 `IPv4 映射的 IPv6 地址`然后调用 `connect`。内核检测到这个映射地址后自动发送一个`IPv4 SYN`到服务器
	- 服务器响应一个`IPv4 SYN/ACK`，连接于是通过使用`IPv4 数据报`建立
## 12.2-12.3 互操作性总结
- `IPv4客户端`与`IPv6服务器(双栈)`
	- `套接字`接受`数据报`分析:
		- 对于`IPv4套接字`它只能接受`IPv4数据报`(`IPv6地址`不能映射为`IPv4地址`)
		- 对于`IPv6套接字(带双栈)`它可以接受`IPv4数据报`和`IPv6数据报`(`IPv4地址`可以被唯一映射为`IPv6地址`)
	- 具体分析: 如果目的地为`IPv6套接字`的`IPv4数据报`,那么内核(`目的地内核`)把该数据包的`源IPv4地址`映射为`IPv6地址`作为`accept(TCP)`或`recvfrom(UDP)`返回的`对端IP地址`
![IPv4客户端与IPv6服务器](https://img-blog.csdnimg.cn/20191028132503169.png#pic_center)
- `IPv4服务器`与`IPv6客户端(双栈)`
- `套接字`发送`数据报`分析:
	- `IPv4数据报`可以接受`IPv4套接字`和`IPv6套接字`的数据发送(`IPv6套接字`的目的地为由`IPv4`地址映射的`IPv6地址`是内核将其转变为`IPv4数据报`)
	- `IPv6数据报`只能接受`IPv6套接字`的数据发送
- `IPv4套接字`不能发送一个`IPv6数据报`,因为不可能往`IPv4套接字`上设置`IPv6地址`,毕竟`IPv4套接字`接受的`sockaddr_in的in_addr成员`只有`4字节`的长度
- 当`IPv6套接字`发送数据时,内核检测到`目的IP地址`为由`IPv4地址`映射的`IPv6地址`,所以此`地址`转换为`IPv4地址`发送`IPv4数据报`
![IPv4服务器与IPv6客户端(双栈)](https://img-blog.csdnimg.cn/20191028132630134.png#pic_center)
---
- 互操作性总结
![互操作性总结](https://img-blog.csdnimg.cn/20191028140308993.png#pic_center)
##  `IPv6`地址测试宏
- 有一些 IPv6 应用必须知道和它通信的是 IPv6 还是 IPv4 协议，使用 <netinet/in.h> 中的函数可以进行测试
- `int IN6_IS_ADDR_V4MAPPED(const struct in6_addr *aptr)宏测试IPv6地址是否由IPv4映射而来`

## 源代码可移植性
- 考虑到源码的可移植性，编写代码时应尽量避免 `gethostbyname`, `gethostbyaddr`等套接字函数，使用 `getaddrinfo`, `getnameinfo`等函数，使得代码变得与协议无关。
## 小结
- 双栈主机的`IPv6 服务器`可以和两种客户端进行通讯, 对于`IPv4客户端`使用`IPv4数据报`进行通信
- 双栈主机的`IPv6 客户端`也可以和两种服务器进行通讯, 对于`IPv4服务器`使用`IPv4数据报`进行通信
# 第13章：守护进程和`inetd`超级服务器 
- `T:2019/10/28  W:1 14:10:40`
## 13.1 概述
> - **守护进程**：是在后台运行且不与任何控制终端关联的进程。`Unix`系统通常有很多守护进程在后台运行(通常为20~50个的数量级)执行不同的管理任务.
> - 守护进程没有终端，通常是因为他们由开机时的脚本进行启动。守护进程也可能从某个终端由用户在`shell`提示符下键入命令行进行启动，这样的守护进程**必须**亲自脱离与控制终端的关联，从而避免与 `作业控制终端`, `会话管理`，`终端产生信号`等发生不希望的交互,也`防止`后台的守护进程`输出到终端`

- 守护进程的启动方式：
	- 在系统阶段进行启动，许多守护进程由系统初始化脚本进行启动，脚本通常位于 `/etc` 等目录，这些脚本启动的守护进程开始就拥有超级用户权限（`inetd`，`Web`，`sendmail`，`syslogd` 等等）
	- 许多网络服务器由 **`inetd`超级服务器(其本身由第一条启动)** 进行启动。`Inetd`监听网络请求，每当有一个请求到达，启动相应的实际服务器`(Telnet，FTP…)`
	- **`cron`守护进程(其本身由第一条启动)** 按规则定期执行一些程序,这些程序同样作为守护继承运行(`单词cron,计时程序`)
	- `at 命令`用于指定将来某个时刻的程序执行，时间到达时，通常使用`corn`来进行执行
	- 守护进程还可以从用户的终端在前台或者后台进行启动。这么做往往是测试守护进程或者重启关闭的守护进程。
- 因为守护进程没有终端，所以他们的消息使用 `syslog` 进行处理，即使用 `syslog` 函数，将消息发送给 `syslogd` 进程
## 13.2 `syslodg`守护进程
- `syslogd`守护进程通常由系统初始化脚本进行启动，并在系统工作时间一直运行，启动步骤如下：
	- 读取配置文件，在 `/etc/syslog.conf` 配置文件指定守护进程收取的各种日志消息应如何处理。可能**添加到一个文件**中，或被**写到用户的登录窗口**，或被**转发给另一个主机上的 `syslogd` 进程**
	- 创建 `Unix 域数据报套接字`，给它捆绑路径名 `/var/run/log`
	- 创建 `UDP 套接字`，捆绑 `514 端口`，接收别的主机发送过来的日志
	- 打开路径名 `/dev/klog`。**来自内核的任何出错消息从这个设备输入**
	- `syslog` 使用 `select` 来监听上面 `2，3，4` 步的描述符来接受日志，并按照配置文件进行处理。如果守护进程读取 `SIGHUP 信号`，就重新读取配置文件
- **最新的系统不建议开启 514 端口，会遭到攻击**
## 13.3 `syslog`
- 守护进程没有终端，所以不能把消息 `fprintf` 到 `stderr` 上。从守护进程中登记消息的常用技巧是调用 `syslog` 函数
- `logger` 命令在 `shell 脚本`中以向 `syslogd` 发送消息
```c
#include <syslog.h>
void syslog(int priority, const char * message, ...);
```
- 参数解析：
	- `priority`：级别(`level`)和设施(`facility`)两者的组合体
	- `message`：类似 `printf` 格式串，增加了 `%m` 规范代表当前 `errno` 值
- 当 `syslog` 被应用进程首次调用时，它创建一个 `Unix 域数据报套接字`，然后调用 `connect` 连接到由 `syslogd` 守护进程创建的 `Unix 域数据报套接字的众所周知的路径名`。这个套接字一直打开，直到进程终止关闭. 可以在`syslog`使用前调用`openlog`,在不需要发送日志时,调用`closelog`(注意`openlog`并不会立即创建套截止,除非指定`NDELAY`选项)
	- `void openlog(const char *ident, int options, int facility);`
	- `void closelog(void);`
	- 参数
		- `ident`是一个由`syslog`冠名的每个日志消息之前的字符串,通常是程序的名字
		- `options`有多和常值的逻辑构成

optiions | 说明
---- | ---
LOG_CONS | 若无法发送到syslogd守护进程则登记到控制台
LOG_NDELAY | 不延迟打开,立即创建套截止
LOG_PERROR | 即发送到syslogd守护进程,又发送到标准错误输出
LOG_PID | 随每个日志消息登记进程ID

- 日志消息的`level`

level | 值| 说明
-----| ---- | ----
LOG_EMERG | 0 | 系统不可用(最高优先级)
LOG_ALERT 	| 1 | 必须立即采取行动
LOG_CRIT		| 2 | 临界条件
LOG_ERR 		| 3 | 出错条件
LOG_WARNING | 4 | 警告条件
LOG_NOTICE | 5 | 正常然而重要的条件(默认值)
LOG_INFO 		| 6 | 通告消息
LOG_DEBUG 	| 7 | 调试级消息(最低优先级)

- 标识消息发送进程类型的`facility`

facility | 说明
-----| -----
LOG_AUTH	| 安全/授权消息
LOG_AUTHPRIV	| 安全/授权消息(私用)
LOG_CRON	| cron守护进程
LOG_DAEMON	| 西东守护进程
LOG_FTP	| FTP守护进程
LOG_KERN	| 内核消息
LOG_LOCAL0	| 本地消息
LOG_LOCAL1	| 本地消息
LOG_LOCAL2	| 本地消息
LOG_LOCAL3	| 本地消息
LOG_LOCAL4	| 本地消息
LOG_LOCAL5	| 本地消息
LOG_LOCAL6	| 本地消息
LOG_LOCAL7	| 本地消息
LOG_LPR	| 行式打印机系统
LOG_MAIL	| 邮件系统
LOG_NEWS	| 网络新闻系统
LOG_SYSLOG	| 由syslogd内部产生的消息
LOG_USER	| 任意的用户级消息(默认)
LOG_UUCP	| UUCP系统

- 例如,当`rename`函数调用失败时,守护进程执行以下调用:
	- `syslog(LOG_INFO|LOG_LOCAL2, "RENAME(%s,%s): %m", file1, file2);`
## 13.4 `daemon_init`
- 编写一个守护进程的创建函数，有些系统(如`Linux`)提供 `daemon` 函数用来创建守护进程，和本程序类似
- 守护进程在没有终端的环境下运行，不会接收 `SIGHUP` 信号。许多守护进程把这个信号可以当作系统发送的通知，表示配置文件发送了变化，应重新读取配置文件，类似的还有 `SIGINT SINGWINCH `信号
```c
#include	"unp.h"
#include	<syslog.h>
#define	MAXFD	64
extern int	daemon_proc;	/* defined in error.c */
int daemon_init(const char *pname, int facility){
	int		i;
	pid_t	pid;
    // 调用 fork 创建子进程，然后直接终止父进程，留下子进程继续执行。
    // 如果是在 shell 中执行的程序，父进程终止，shell 会认为程序已经结束了，子进程就可以在后台执行了
    // 子进程继承父进程的进程组 ID，但它有自己的进程 ID，这就保证了子进程不是一个进程组的头进程，这是接下来调用 setsid 的必要条件
	if ( (pid = Fork()) < 0)
		return (-1);
	else if (pid)
		_exit(0);			/* parent terminates */

	/* child 1 continues... */
    // setsid 用来创建一个新的会话。当前进程变为新会话的会话头进程以及新进程组的进程组头进程，从而不再有控制终端
	if (setsid() < 0)			/* become session leader */
		return (-1);
    // 忽略 SIGHUP 信号，并再次调用 fork。该函数返回时，同样只使用子进程，父进程返回
    // 再次 fork 是为了确保本守护进程将来即使打开一个新的终端，也不会自动获得控制终端。
    // 当没有终端的一个会话头进程打开终端时，该终端自动成为这个头进程的控制终端。
    // 再次调用 fork，产生的子进程不是会话头进程，就不会自动获得一个控制终端。
    // 这里必须忽略 SIGHUP 信号，当会话头进程终止时，所有会话子进程都会收到 SIGHUP 信号
	Signal(SIGHUP, SIG_IGN);
	if ( (pid = Fork()) < 0)
		return (-1);
	else if (pid)
		_exit(0);			/* child 1 terminates */

	/* child 2 continues... */
    // 把全局变量 daemon_proc 设置为非 0 值，这个变量由 err_XXX 函数使用，不为 0 是为了告诉他们将 fprintf 输出替换为调用 syslog 函数
	daemon_proc = 1;			/* for err_XXX() functions */
    // 改变工作目录到根目录
	chdir("/");				/* change working directory */

	/* close off file descriptors */
    // 关闭所有打开的描述符，直接关闭前 64 个，这里不考虑太多
	for (i = 0; i < MAXFD; i++)
		close(i);

	/* redirect stdin, stdout, and stderr to /dev/null */
    // 将 stdin stdout stderr 重定向到 /dev/null
	open("/dev/null", O_RDONLY);
	open("/dev/null", O_RDWR);
	open("/dev/null", O_RDWR);
    // 使用 syslogd 处理错误
	openlog(pname, LOG_PID, facility);

	return (0);				/* success */
}
```
- 例子:时间服务器做守护进程
```c
#include "unp.h"
#include <time.h>
int mian(int argc, char **argv){
	int listenfd, connfd;
	socklen_t addrlen, len;
	struct sockaddr *cliaddr;
	char buff[MAXLINE];
	time_t ticks;
	if(argc <2 || arc > 3) err_quit("usage: dayimetcpsrv2 [ <host> ] <service or port>");
	daemon_init(argv[0], 0);
	if(argc ==2) Tcp_listen(NULL, argc[1], &addrlen);
	else Tcp_listen(argc[1], argc[2], &addrlen);
	cliaddr = Malloc(addrlen);
	while(1){
		len = addrlen;
		connfd = Accept(listenfd, cliaddr, &len);
		err_msg("connect from %s", Sock_ntop(cliaddr, len));
		ticks = time(NULL);
		snprintf(buff, sizeif(buff), "%.24f\r\n", ctime(&ticks));
		Write(connfd, buff, strlen(buff));
		Close(connfd);
	}
}
```
## 13.5 `inetd`守护进程
> - `Unix`系统中可能存在很多服务器,他们只是等待客户请求的到大,如`FTP`, `Telnet`, `Rlogin`等等. 这些进程都是在系统自举阶段从`/etc/rc`文件中启动,而且每个进程执行几乎相同的启动任务: 创建一个套接字,把本服务器的监听端口绑定到套接字上,等待客户连接,然后派生子进程.子进程为客户提供服务.
> - 交给 `inetd` 守护进程来进行处理，既可以简化编写的代码，又可以对每次客户端的请求单独响应，并不需要每个服务器一直等待客户端，因为他们大多数时间都处于休眠状态。只需要 inetd 循环等待客户端的请求即可，来了请求，为对应的客户创建需要的服务器子进程即可。

- 这个模型存在的问题:
	- 所有的这些守护进程几乎有相同的启动代码,如创建套接字上以及升级为守护进程
	- 这些进程大部分时间都处于休眠状态
- 使用英特网超级服务器(`inetd`守护进程)使上述问题得到简化:
	- 将`inetd`升级为守护进程
	-  `inetd`循环等待客户端的请求即可，来了请求，为对应的客户创建需要的服务器子进程即可.
- `inetd`首先把自己升格为`守护进程`,然后读入并处理配置文件(通常是`/etc/inetd.conf`)该文件每一行的字段如下:

- `inetd.conf`文件中的子段

字段| 说明
:---- | :----
service-name | 	必须在/etc/services文件中定义
socket-type |		stream(对于TCP)活dgram(对于UDP)
protocal	| 			必须在/etc/protocals文件中定义: TCP/UDP
wait-flag | 对于TCP一般为nowait, 对于UDP一般为wait
login-name | 来自/etc/passwd的用户名,一般为root
server-program | 调用exec指定的完整路径名
server-program-arguments|调用exec指定的命令行参数

- 例如(**标题是为了方便理解添加的**):

service-name |socket-type |protocal	|wait-flag | login-name | server-program | server-program-arguments
-----| -----| -----| ----- | ----- | ----- | ---
ftp | stream | tcp | nowait | root | /usr/bin/ftpd | ftpd -l
telnet | stream | tcp | nowait | root | /usr/bin/telnetd | telnetd
login | steam| tcp | nowait | root | /usr/bin/rlogind | rlogind -s
tftp | dgram | udp | wait | nobody | /usr/bin/tftpd | tftpd -s /tftpboot

- 当使用 inetd 调用`exec`指定某一个服务器程序时，该服务器的真实名字总是作为程序的第一个参数传递
![inetd的工作流程](https://img-blog.csdnimg.cn/20191028190632679.png#pic_center)
- `inetd` 工作流程(数据包服务:`nowait`)
- 1. 启动阶段，读取配置文件，并给文件中每个类型服务器创建一个适当的类型（`TCP or UDP`…）的套接字。`inetd` 能够处理的服务器最大个数取决于 `inetd` 能够创建的描述符最大个数，使用 select 对所有描述符进行集中
- 2. 为每个套接字调用 `bind`，指定 `IP + port`。端口通过 `getservbyname` 获取
- 3. 对于 `TCP 套接字`，调用 `listen` 来进行监听，`UDP` 不用执行
- 4. 使用 `select` 对所有套接字描述符进行监听，`inetd` 大部分时间都花在这里
- 5. 如果可读的是 `TCP` 套接字描述符，调用 `accept` 来进行连接
- 6. 调用 `fork` 创建子进程来处理不同的请求，类似于并发服务器
- 7. 如果第`5`步返回字节流套接字，父进程要关闭已连接套接字，就是`accept`的套接字，类似于`TCP`并发服务器
- `ined`工作流程(数据包服务:`wait`)与数据包`nowait`的差异, 对于数据报服务指定为`wait`标志导致父进程执行步骤发生变化. 这个标志要求`inetd`必须在这个套接字再次成文`select`调用的候选套接字之前等待等待当前服务该套接字的子进程终止.发生的标有如下几点:
- 1. `fork`返回到父进程时,父进程保存`子进程的ID`. 这么做使得父进程能够通过查看由`waitpid`返回的值确定这个子进程的终止时间
- 2. 父进程通过使用`FD_CLR`宏关闭这个套接字在`select`所在描述符集对应位,达成在将来`select`调用中禁止这一套接字的目的.
- 3. 当子进程终止时, 父进程被通知以一个`SIGCHLD`信号,而父进程的信号处理函数将取得该子进程的`进程ID`父进程随即打开套接字在`select`所在描述符集中对应位,使得此套接字重新成为`select`的候选套接字
## 13.6 `daemon_inetd`
- 该函数可以用在 `inetd` 启动的服务器程序中
```c
#include	"unp.h"
#include	<syslog.h>
extern int	daemon_proc;	/* defined in error.c */
void daemon_inetd(const char *pname, int facility){
	daemon_proc = 1;		/* for our err_XXX() functions */
	openlog(pname, LOG_PID, facility);
}
```
- 所有的步骤已经由 `inetd` 在启动时执行完毕，本函数仅仅处理错误函数设置 `daemon_proc` 标志，并调用 `openlog` 函数
- 由 `inetd` 作为守护进程启动时间获取服务器程序
```c
#include	"unp.h"
#include	<time.h>
int main(int argc, char **argv) {
	socklen_t		len;
	struct sockaddr	*cliaddr;
	char			buff[MAXLINE];
	time_t			ticks;
	daemon_inetd(argv[0], 0);

	cliaddr = Malloc(sizeof(struct sockaddr_storage));
	len = sizeof(struct sockaddr_storage);
	Getpeername(0, cliaddr, &len); // 因为inetd会关闭除accept返回套截止的所有其他描述符,然后把返回套截止复制至 0(标准输入),1(标准输出),2(标准错误)
	err_msg("connection from %s", Sock_ntop(cliaddr, len));

    ticks = time(NULL);
    snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
    Write(0, buff, strlen(buff));

	Close(0);	/* close TCP connection */
	exit(0);
}
```
- 小结
	- 守护进程是在后台运行并独立与所有终端的进程，许多网络服务器作为守护进程运行。守护进程所产生的输出调用 `syslog` 函数交给 `syslogd` 守护进程处理
	- 启动任意一个程序并将其变为守护进程步骤如下：
		- 调用 `fork` 到后台运行
		- 调用 `setsid` 创建一个新会话，并让前一步的**子进程成为会话头进程**
		- 再次 `fork` 防止会话头进程自动获取控制终端(当没有终端的一个会话头进程打开终端时，该终端自动成为这个头进程的控制终端。再次调用 fork，产生的子进程不是会话头进程，就不会自动获得一个控制终端)
		- 改变工作目录
		- 创建模式掩码(`???`)
		- 关闭所有非必要描述符
		- 许多 `Unix 服务器`由 `inetd` 守护进程启动。它处理所有守护进程需要的步骤，当启动真正的服务器时，套接字已在标准输入，标准输出，标准错误上打开。这样就不用调用 `socket`，`bind`，`accept`，这些步骤已经由 `inetd` 完成。
# 第14章：高级`IO`函数
##  14.1 概述
- 本章讨论 `I/O` 的高级操作，首先是在 `I/O` 上设置超时，这里有三种方法。
- 然后是 `read` 和 `write` 的三个变体：
	- `recv` 和 `send` 允许通过第四个参数从进程到内核传递标志
	- `readv` 和 `writev` 允许指定往其中输入数据或从其中输出数据的缓冲区向量
	- `recvmsg` 和 `sendmsg` 结合了其他 `I/O` 函数的所有特性，并具备接收和发送辅助数据的新能力
## 14.2 套接字超时
- 设置超时的方法有 3 种：
	- 调用 `alarm`，它在指定超时期满时产生 `SIGALRM` 信号。这个方法涉及信号处理，而信号处理在不同的实现上存在差异，而且可能干扰进程中现有的 `alarm` 调用
	- 在 `select` 中阻塞等待 `I/O`（`select 有内置时间限制`）以此代替直接阻塞在 `read` 或 `write` 调用上
	- 使用较新的 `SO_RCVTIMEO` 和 `SO_SNDTIMEO` 套接字选项。这个方法的问题在于并非所有实现都支持这两个套接字选项。
- 上述三个技术都适用于输入和输出操作（例如 `read`，`write` 以及诸如 `recvfrom`，`sendto` 之类），不过我们依然期待可用于 `connect` 的技术，因为 `TCP` 内置的 `connect` 超时相当长。**`select `可用来在 `connect` 上设置超时的先决条件是相应套接字处于非阻塞模式**，而那两个套接字选项对 `connect` 并不适用。我们还指出，前两个技术使用于任何描述符，而第三个技术仅仅适用于套接字描述符（因为是套接字描述符选项）
## 使用 `SIGALRM` 为 `connect` 设置超时 
+  本方法仅能减少 `connect` 的超时，但**不能增加 connect 的超时设置**，因为 connect 有自己的超时设置(例如先有内核的超时时长为`75s`, 如果我们调用函数设置`10s(小于原时长的值)`, 都是行得通的,但是当设置为`80s(大于原时长的值)`,则会失败,依旧在`75s(等于原时长)`时发生超时)
+ **需要注意的是在多线程中使用信号非常困难，建议仅仅在未线程化或仅在单线程中使用本技术**
```c
static void	connect_alarm(int);
int connect_timeo(int sockfd, const SA *saptr, socklen_t salen, int nsec){
	Sigfunc	*sigfunc;
	int		n;
    // 设置 SIGALRM 信号的处理函数，并保存原有的处理函数到 sigfunc
	sigfunc = Signal(SIGALRM, connect_alarm);
    // 设置报警时钟的秒数，返回值是上一次设置的剩余秒数，没有就返回 0
	if (alarm(nsec) != 0) err_msg("connect_timeo: alarm was already set");
    // connect 为慢调用,当被终端打断时,就会发出返回,并置 errno 为EINTR
    // 调用 connect，调用中断就设置 error 设置为 TIMEOUT，并关闭套接字，防止三路握手继续进行
	if ( (n = connect(sockfd, saptr, salen)) < 0) {
		close(sockfd);
		if (errno == EINTR) errno = ETIMEDOUT; 
	}
    // 关闭报警时钟，并恢复原处理函数
	alarm(0);					/* turn off the alarm */
	Signal(SIGALRM, sigfunc);	/* restore previous signal handler */
	return(n);
}

// 信号函数仅仅返回
static void connect_alarm(int signo)
{
	return;		/* just interrupt the connect() */
}
```
## 使用 `SIGALRM` 为 `recvfrom` 设置超时
- 本例子工作正常,因为每次读取`alarm`设置报警时钟之后,期待读取的只是单个应答
```c
static void	sig_alrm(int);
void dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen){
	int	n;
	char	sendline[MAXLINE], recvline[MAXLINE + 1];
	Signal(SIGALRM, sig_alrm); // 为什么保留原来的中断处理函数,结束后也没有恢复

	while (Fgets(sendline, MAXLINE, fp) != NULL) {
		Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);
        // 调用 recvfrom 函数前设置了 5 秒的超时设置
		alarm(5);
		if ( (n = recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL)) < 0) {
			if (errno == EINTR)
				fprintf(stderr, "socket timeout\n");
			else
				err_sys("recvfrom error");
		} else {
            // 读取数据，关闭超时处理
			alarm(0);
			recvline[n] = 0;	/* null terminate */
			Fputs(recvline, stdout);
		}
	}
}
// 简单返回，用来中断阻塞的 ercvfrom 调用
static void sig_alrm(int signo)
{
	return;			/* just interrupt the recvfrom() */
}
```
## 使用 `select` 为 `recvfrom` 设置超时
- 该函数中 `select` 指定等待描述符的最长时间
```c
int readable_timeo(int fd, int sec)
{
	fd_set			rset;
	struct timeval	tv;
    // 准备 select 参数
	FD_ZERO(&rset);
	FD_SET(fd, &rset);

	tv.tv_sec = sec;
	tv.tv_usec = 0;
    // 调用有超时的 select 函数，出错返回 -1，超时返回 0
    // 本来select 还应该更具返回值判断是属于那个标志位可读,但是这里集合只有一个描述符,所以就不用判断了
	return(select(fd+1, &rset, NULL, NULL, &tv));
		/* 4> 0 if descriptor is readable */
}
```
## 使用 `SO_RCVTIMEO` 套接字选项为 `recvfrom` 设置超时
- 该操作设置一次即可，与套接字的读操作绑定，前面的方法都需要循环重新设置，
- **本套接字选项仅适用于读操作**，类似的 **`SO_SNDTIMEO` 选项对应于写操作**，两者均不能用于 `connect` 设置超时
```c
void dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
	int				n;
	char			sendline[MAXLINE], recvline[MAXLINE + 1];
	struct timeval	tv;
    // 指向 timeval 结构体的指针，保存的是超时的值
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	Setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	while (Fgets(sendline, MAXLINE, fp) != NULL) {
		Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);
		n = recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
		if (n < 0) {
            // I/O 超时操作，recvfrom 函数返回一个 EWOULDBLOCK 错误
			if (errno == EWOULDBLOCK) {
				fprintf(stderr, "socket timeout\n");
				continue;
			} else
				err_sys("recvfrom error");
		}
		recvline[n] = 0;	/* null terminate */
		Fputs(recvline, stdout);
	}
}
```
## 14.3 `recv` 和 `send` 函数
- 类似于 `read` 和 `write` 函数，不过多一个参数 `flags`
```c
#include<sys/socket.h>
ssize_t recv(int sockfd, void* buff, size_t nbytes, int flags);
ssize_t send(int sockfd, const void* buff, size_t nbytes, int flags);
// 返回：成功返回读入写出的字节数，出错返回 -1
```
- `flag` 可以用来标识一些操作，**绕过路由表查找**、**仅本操作非阻塞**、**发送或接受外带数据**、**窥看外来消息**、**等待所有数据**。
- `flag` 是值传递，并不是值-结果参数。所以它只能从进程向内核传递标志，内核不能返回标志。
	- 随着协议的增加，有时候需要值-结果参数(内核向进程返回错误消息), 这个操作被融入到 `recvmsg` 和 `sendmsg` 中用 `msghdr` 
## 14.4 `readv` 和 `writev` 函数
- 这两个函数类似 `read` 和 `write`，不过 `readv` 和 `writev` 允许单个系统调用读入或写出自一个或者多个缓冲区。这些操作被称为分散读和集中写，因为来自读操作的输入数据被分散到多个应用缓冲区中，而来自多个应用缓冲区的输出数据被集中提供给单个写操作。
```c
#include<sys/uio.h>
ssize_t readv(int fileds, const struct iovec* iov, int iovcnt);
ssize_t writev(int fields, const struct iovec* iov, int iovcnt);
// 返回：成功返回读入或写出的字节数，出错返回 -1
struct iovec{
    void *iov_base;      // buf 的开始地址
    size_t iov_len;      // buf 的大小
}
```
- `struct iovec` 结构体定义如上，函数第二个参数指向的是一个 `iovec 的数组`，一般系统中定义数组长度的常值为 `16`，最大值一般在 `1024~2100`
- `readv` 和 `writev` 函数可用于任何描述符，不仅仅局限于套接字描述符。`writev` 是一个原子操作，所以对于 `UDP` 来说，一次 `writev` 仅产生一个 `UDP 数据报`
- **当一个 `4 字节`的 `write` 和 `396 字节`的 `write` 调用时可能会触发 `naggle 算法`合并它们，解决这个问题的首选方法就是针对这两个缓冲区调用 `writev` 函数**
## 14.5 `recvmsg` 和 `sendmsg` 函数
- 这两个函数是最通用的 `I/O` 函数，可以替换上面所有的读写函数
```c
#include<sys/socket.h>
ssize_t recvmsg(int sockfd, struct msghdr * msg, int flags);
ssize_t sendmsg(int sockfd, struct msghdr * msg, int flags);
// 返回：成功读入或者写出的字节数，出错则为 -1
struct msghdr{   // 用来保存大部分参数
    void *mag_name;
    socklen_t msg_namelen;
    struct iovec *msg_iov;
    int msg_iovlen;
    void *msg_control;
    socklen_t msg_controllen;
    int msg_flags;
}
```
- 1. `mag_name` 和 `msg_namelen` 这两个成员用于套接字未连接的场合`(UDP 等)`，`msg_name` 指向一个套接字地址结构，在其中存放接受者`(sendmsg)`或者发送者`(recvmsg)`的协议地址。如果无需指明协议地址，`msg_name` 是空指针。`msg_namelen` 对于 `sendmsg` 是值参数，对于 `recvmsg` 是值-结果参数。
- 2. `msg_iov` 和 `msg_iovlen` 这两个成员指定输入或输出缓冲区数组（就是 `iovec` 数组, 在上一小节中定义）
- 3. `msg_control `和 `msg_controllen` 这两个成员指定可选的辅助数据的位置和大小 `msg_controllen` 对于 `recvmsg` 来说是一个值-结果参数
- 4. 对于 `recvmsg` 和 `sendmsg` 需要区别他们的两个标志变量，一个是传递值的 `flags` 参数，另一个是所传递 `msghdr` 结构的 `msg_flags` 成员，传递的是引用，因为传递给函数的是地址
- 5. 只有 `recvmsg` 使用 `msg_flags` 参数。`recvmsg`被调用时, `flags`参数被复制到`msg_flags`成员,并由内核使用其值驱动接受处理过程. 内核依旧使用`recvmsg`的结果更新`msg_flags`
- 6. `sendmsg` 忽略该参数，它直接使用`flag`参数驱动发送过程
![IO函数的输入输出标志总结](https://img-blog.csdnimg.cn/20191029133309668.png#pic_center =700x300)
<br>
![UDP套接字调用recvmsg时的数据结构](https://img-blog.csdnimg.cn/20191029133501824.png#pic_center)
- `UDP套接字`调用`recvmsg`时的数据结构
	-  协议地址分配空间为`16字节`, 辅助数据分配空间为`20字节`
	-  缓冲区初始化分配空间为`3`个`iovec结构`组成的数组
		- 第一个为`100字节`的缓冲区
		- 第二个为`60字节`的缓冲区
		- 第三个为`80字节`的缓冲区
	-  `msg_flags`是由函数内部copy`flags`过去的,所以这里为空

<br>

![recvmsg返回时的更新](https://img-blog.csdnimg.cn/20191029134249875.png#pic_center)
- `recvmsg`返回时的更新
	- 由`msg_name`成员指向缓冲区被一个网际套接字地址结构填充,其为收到数据报的`源IP`和`UDP源端口`
	- `msg_namelen`成员(值-结果参数)被更新为网络套接字地址结构的长度,但是这里无变化,本来就是`16字节`
	- 所接受数据报的前`70字节`存放在第一个缓冲区中,`中60字节`存放在第二个缓冲区中, `后10字节`存放在第三个缓冲区中,第三个缓冲区的`最后70字节`无变化(`recvmsg`函数的返回值,即`170`,就是该数据报的大小)
	- 由`msg_control`成员指向缓冲区被填充了一个`cmsghdr结构`.
		- 该结构的`cmsg_len`成员值为`16`
		- 该结构的`cmsg_level`成员值为`IPPROTO_IP`
		- 该结构的`cmsg_type`成员值为`IP_RECVDSTADDR`
		- 随后`4字节`存放所收到的`UDP数据报`的`目的IP地址`,
		- 这`20字节的缓冲区`的后`4字节`没有动
-  五组 `I/O` 函数之间的差异
<h3 id="五组-I-O-函数之间的差异"><a href="#五组-I-O-函数之间的差异" class="headerlink" title="五组 I/O 函数之间的差异"></a>五组 I/O 函数之间的差异</h3><table>
<thead>
<tr>
<th>函数</th>
<th>任何描述符</th>
<th>仅套接字描述符</th>
<th>单个读/写缓冲区</th>
<th>分散/集中读/写</th>
<th>可选标志</th>
<th>可选对端地址</th>
<th>可选控制信息</th>
</tr>
</thead>
<tbody><tr>
<td>read, write</td>
<td>√</td>
<td></td>
<td>√</td>
<td></td>
<td></td>
<td></td>
<td></td>
</tr>
<tr>
<td>readv, writev</td>
<td>√</td>
<td></td>
<td></td>
<td>√</td>
<td></td>
<td></td>
<td></td>
</tr>
<tr>
<td>recv, send</td>
<td></td>
<td>√</td>
<td>√</td>
<td></td>
<td>√</td>
<td></td>
<td></td>
</tr>
<tr>
<td>recvfrom, sendto</td>
<td></td>
<td>√</td>
<td>√</td>
<td></td>
<td>√</td>
<td>√</td>
<td></td>
</tr>
<tr>
<td>recvmsg, sendmsg</td>
<td></td>
<td>√</td>
<td></td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
</tr>
</tbody></table>

## 14.6 辅助数据
- 辅助数据可通过调用 `sendmsg` 和 `recvmsg` 这两个函数，使用 `msghdr` 结构中的 `msg_control` 和 `msg_controllen` 这两个成员来发送和接收，也叫做控制信息。
- 辅助数据由一个或多个辅助数据对象构成，每个对象以一个定义在头文件`<sys/socket.h>`中的 `cmsghdr` 结构体
	```c
	struct cmsghdr{
		socklen_t cmsg_len;
		int cmsg_level;
		int cmsg_type;
	}
	```
![辅助数据的用途的总结](https://img-blog.csdnimg.cn/20191029140514341.png#pic_center)
## 14.7 排队的数据量
> - 有时候我们想要在不真正读取数据的前提下知道一个套接字上已有多少数据排队等待着读取。
- 有三种技术可以获得一排队数据量。
	- 如果**获取排队数据量的目的在于避免读操作内核阻塞**，那么可以使用`非阻塞式 I/O`，但是不能获得数据量，只能直到是否有数据。
	- 如果我们**既想查看数据又想数据留在接收队列中等待其余部分的读取**，使用 `MSG_PEEK` 标志。
		- 如果我们想这么做,但是不知道是否真的额有数据可读:
			- 可以使用非阻塞套接字来实现既能判断是否有数据可读，然后又可以查看数据但不读取(`MSG_PEEK`)。
			- 需要注意的是对于 `TCP` 连接，两次获取量的值大小可能不同，如果在两次获取之间收到了流数据。
			- 但是 `UDP` 仅返回第一个数据报的大小，所以即使两次之间有新的数据报，也不影响
	- 一些实现支持 `ioctl` 的 `FIONREAD` 命令。该命令的第三个 `ioctl` 参数是指向某个整数的一个指针，内核通过该整数返回的值就是套接字接收队列的当前字节数。该值是已排队字节的总和，对于 `UDP` 包括所有已排队的数据报。**某些实现中，对 `UDP` 套接字返回的值还包括一个套接字地址结构的空间，其中含有发送者的 `IP 地址和端口号`**
## 14.8 套接字和标准 `I/O`
- 执行 `I/O` 还可以使用标准 `I/O` 函数库，使用标准 `I/O` 对套接字进行读取一般可以打开两个流，一个用来读，一个用来写。
- **不建议在套接字上使用标准 `I/O`**
## 14.9 高级轮询技术
## `kqueue` 接口
- 本接口允许进程向内核注册描述所关注的 `kqueue` 事件的事件过滤器。事件除了与 `select` 所关注类似的`文件 IO` 超时外，还有`异步 IO`、文件修改通知、进程跟踪、信号处理
	```cpp
	#include<sys/types.h>
	#include<sys/event.h>
	#include<sys/time.h>
	int kqueue(void);
	int kevent(int kq, const struct kevent * changelist, int nchanges,
				struct kevent * eventlist, int nevents,
				const struct timespec * timeout);
	void EV_SET(struct kevent *kev, uintptr_t ident, short filter, 
				u_short flags, u_int fflags, intptr_t data, void *udata);
	
	struct kevent{
		uintptr_t ident;
		short filter;
		u_short flags;
		u_int fflags;
		intptr_t data;
		void *udata;
	}
	```
- `kqueue` 函数返回一个新的 `kqueue 描述符`，用于后面的 `kevent 调用`。
- `kevent` 函数即用于注册所关注的事件，也用于确定是否有所关注事件发生。
- `changelist` 和 `nchanges` 这两个参数给出对所关注事件做出更改，没有的话设置为 `NULL`，`0`。
- 关于 `timeout 结构体`的区别，`select` 是纳秒，而 `kqueue` 是微秒。

- **使用实例(见书上 333页)**

## 14.10 `T/TCP`：事务目的 `TCP`
- `T/TCP` 是对 `TCP` 的略微修改，避免最近通信过的主机之间再次三次握手。它能把 `SYN`，`FIN` 和数据组合到单个分节中，前提是一个分节可以存储这些数据。
![最小T/TCP事务的时间线](https://img-blog.csdnimg.cn/20191029155257552.png#pic_center)
- 最小T/TCP事务
	- 第一分节 是由于客户端发起的单个`sendto`调用产生`SYN`,`FIN`和数据,该分节组合了`connect`,`write`和`shutdown`三个调用的功能
	- 服务器执行通常的套接字函数调用步骤: `socket`,`bind`,`listen`,和`accept`,然后在客户端分节到达时返回
		- 服务器用`send`发挥应答并关闭套接字.服务器在同一分节中向客户端发出了`SYN`,`FIN`和应答

- `T/TCP` 的优势在于`TCP`的所有可靠性(序列号,超时,重传,等等)得以保留,而不像`UDP`那样把可靠性推给应用程序实现. `T/TCP`同样维持`TCP`的慢启动和拥塞避免措施,`UDP`应用程序往往缺乏拥塞避免措施
- `T/TCP` 包含所有 `TCP` 的特性，使得基于 `TCP` 的连接有了类似于 `UDP` 的效果，即两个主机之间频繁连接断开，但是使用 `T/TCP` 可以使得三次握手的消耗几乎为 `0`

## 小结
- 在套接字操作上设置时间限制的方法有三个：
	- 使用 `alarm` 函数和 `SIGALRM` 信号。
	- 使用由 `select` 提供的时间限制
	- 使用较新的 `SO_RCVTIMEO` 和 `SO_SNDTIMEO` 套接字选项
- 第一种方法简单易用，但是涉及信号处理，可能引发竞争条件。使用 `select` 会阻塞在 `select` 上，而不是阻塞在 `read`，`write`，`connect` 调用上。第三种方法不是所有系统都提供。
- `recvmsg` 和 `sendmsg` 是 5 组读写函数中最通用的。它有其余读写函数的所有特性：指定 `MSG_xxx`，返回或指定对端的协议地址，使用多个缓冲区，还增加了两个新特性：给应用进程返回标志，接收或者发送辅助数据。
- `C` 标准 `I/O` 可以用在套接字上，但是并不推荐使用。
- `T/TCP` 是对 `TCP` 的简单增强版本，能够在两台主机最近通信的前提下避免三路握手，使得对端更快的做出应答。从编程角度看，客户端通过调用 `sendto` 而不是通常使用的 `connect write shutdown` 调用序列发挥 `T/TCP` 的优势
