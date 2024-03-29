+++
title = "Linux Signal Examples"
date = 2018-11-13T21:38:21+08:00
lastmod = 2019-04-23T16:13:41+08:00
tags = ["signal", "sigaction", "alarm", "kill", "raise"]
categories = ["NOTE"]
draft = false
+++

信号是系统响应某些条件而产生的一个事件，接收到该信的进程做出相应的处理。通常信是由错误产生的，如段错误(`SIGSEGV`)。 但信还可以作为进程间通信的一种方式，由一个进程发送给另一个进程。

信号定义在 `signal.h` 文件中，以 `SIG` 作为开头，可用 `kill -l` 命令查看，详细信息参见 [man 7 signal](http://man7.org/linux/man-pages/man7/signal.7.html) 。


## 信号处理 {#信号处理}

信号可以通过 `signal` 和 `sigaction` 函数来注册处理， `signal` 函数是 `struct sigaction` 中 `sa_handler` 的一种便捷实现。


### `signal` 函数 {#signal-函数}

<!--more-->

**原型：**

```c
void (*signal(int sig, void (*func)(int)))(int);
```

其中 `sig` 是需要捕获的 `signal number`, 后一个是捕获到信号后的处理函数指针，所以处理函数的原型必须是 `void func(int)` ，简单的代码示例如下：

```c
#include <stdio.h>
#include <string.h>
#include <signal.h>

static void
handler(int sig)
{
        printf("Recieved signal: %d\n", sig);
}

int
main(int argc, char *argv[])
{
        signal(SIGINT, handler);

        printf("Caught SIGINT, input 'quit' to exit...\n");
        // wait signal caught
        char buf[1024] = {0};
        while (1) {
                printf("Please input: ");
                scanf("%s", buf);
                if (strcmp(buf, "quit") == 0) {
                        break;
                }
        }
        printf("Exit...\n");
        return 0;
}
```

另外 `api` 中也提供了下面 2 个特殊的 `handler`:

-   `SIG_IGN`

    忽略此信号

-   `SIG_DFL`

    恢复此信号的默认行为


### `sigaction` 函数 {#sigaction-函数}

**原型：**

```c
int sigaction(int sig, const struct sigaction *restrict act,
           struct sigaction *restrict oact);
```

其中 `sig` 为 `signal number`, `act` 指定信号的处理行为， `oact` 如果不为 `NULL` 则返回信号之前的处理行为。

`struct sigaction` 的主要成员如下：

| 类型                                   | 名称          | 描述                                  |
|--------------------------------------|-------------|-------------------------------------|
| void(\*) (int)                         | sa\_handler   | 处理函数指针，同 signal 函数中的 `func` 参数 |
| sigset\_t                              | sa\_mask      | 信号屏蔽字，是指当前被阻塞的一组信号，不能被当前进程收到 |
| int                                    | sa\_flags     | 处理行为修改器，指明哪种处理函数生效，详见下文 |
| void(\*) (int, siginfo\_t \*, void \*) | sa\_sigaction | 处理函数指针，仅 sa\_flags == SA\_SIGINFO 时有效 |

其中 `sa_flags` 主要可以设置为以下值：

-   SA\_NOCLDSTOP

    子进程停止时不产生 `SIGCHLD` 信号

-   SA\_RESETHAND

    将信号的处理函数在处理函数的入口重置为 `SIG_DFL`

-   SA\_RESTART

    重启可中断的函数而不是给出 `EINTR` 错误

-   SA\_SIGINFO

    使用 `sa_sigaction` 做为信号的处理函数

-   SA\_NODEFER

    捕获到信号时不将它添加到信号屏蔽字中

简单的代码示例如下：

```c
#include <stdio.h>
#include <string.h>
#include <signal.h>

#define SIG SIGINT

static void
sig_handler(int sig, siginfo_t *si, void *data)
{
        printf("Caught signal: %d\n", sig);
        printf("Sender pid: %d\n", si->si_pid);
        printf("Sender uid: %d\n", si->si_uid);
}

static int
sig_caught(int sig)
{
        printf("Start caught signal: %d\n", sig);
        struct sigaction sa;
        sa.sa_flags = SA_SIGINFO;
        sa.sa_sigaction = sig_handler;
        sigemptyset(&sa.sa_mask);
        int ret = sigaction(sig, &sa, NULL);
        if (ret == -1) {
                printf("Failed to caught signal: %d\n", sig);
                return -1;
        }

        return 0;
}

int
main(int argc, char *argv[])
{
        if (sig_caught(SIG) == -1) {
                return -1;
        }

        printf("Caught signal(%d), input 'quit' to exit...\n", SIG);
        char buf[1024] = {0};
        while(1) {
                printf("Please input: ");
                scanf("%s", buf);
                if (strcmp(buf, "quit") == 0) {
                        break;
                }
        }
        printf("Exit...\n");
        return 0;
}
```


### 信号屏蔽字 {#信号屏蔽字}

考虑一下这种情况：在 `signal()/sigaction()` 返回之前进程就已经收到了需要处理的信号，此时进程会以默认行为来处理，这显然不符合我们的期望。 这时就需要用到信号屏蔽字了，在进程启动时就将需要处理的信号加入的屏蔽字中，等 `signal()/sigaction()` 返回后再解除屏蔽，解除屏蔽后至少会将收到的待处理信号发送一个给进程。

屏蔽字用到一下函数：

```c
int sigemptyset(sigset_t *set);
int sigaddset(sigset_t *set, int signo);
int sigprocmask(int how, const sigset_t *restrict set,
           sigset_t *restrict oset);
```

`sigprocmask` 中 `set` 为需要设置的屏蔽字集， `oset` 为之前的屏蔽字集， `how` 控制着 `set` 如何生效，可设置为以下值：

-   SIG\_BLOCK

    该进程的屏蔽字集将为当期屏蔽字集与 `set` 的并集， `set` 中包含了需要屏蔽的信号集

-   SIG\_UNBLOCK

    该进程的屏蔽字集将为当期屏蔽字集与 `set` 的补集的交集， `set` 中包含了需要解除屏蔽的信号集

-   SIG\_SETMASK

    该进程的屏蔽字集将设置为 `set` 的值

简单的设置流程如下：

```c
int
sig_block(int sig, int how)
{
        sigset_t mask;
        sigemptyset(&mask)
        sigaddset(&mask, sig);
        sigprocmask(how, &mask, NULL);
}
```


## 信号发送 {#信号发送}

信号可以通过 `kill` 函数发送给指定进程，也可以通过 `raise` 或者 `alarm` 函数发送给当前执行的线程或进程，下面来分别说说这几个函数。


### kill {#kill}

**原型：**

```c
int kill(pid_t pid, int sig);
```

`kill` 函数向指定进程发送指定的信号，如果信号为 0 将执行错误检查，信号并不会发送，可以用来检查 `pid` 的有效性。

`pid` 大于 0 时信号将发送给此进程， `pid` 小于等于 0 时，如下：

-   等于 0

    信号将发送给发送者所在组里的所有进程

-   等于 -1

    信号将发送给所有进程

-   小于 -1

    信号将发送给进程组为 `pid` 绝对值的所有组内进程


### alarm {#alarm}

**原型：**

```c
unsigned alarm(unsigned seconds);
```

`alarm` 函数将在指定的 `seconds` 之后发送一个 `SIGALRM` 信号，如果 `seconds` 为 0, 则取消之前的定时器请求。如果不为 0 则取消之前的请求，重新设置为 `seconds` 。 如果在等待结束之前有其他的事件产生，那定时器请求也将被取消。

简单的代码示例如下：

```c
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

static void
handler(int sig)
{
        printf("alarm arrived: %d\n", sig);
}

int
main(int argc, char *argv[])
{
        signal(SIGALRM, handler);

        alarm(2);

        sleep(2);
        printf("alarm 5s over\n");

        alarm(10);
        sleep(1);

        unsigned int remaining = alarm(3);
        printf("alarm 10s remain: %u, reset to 3\n", remaining);
        sleep(3);
        printf("alarm 3s over\n");

        alarm(20);
        sleep(3);

        remaining = alarm(0);
        printf("cancel alarm 20s, remian: %u, exit...\n", remaining);
}
```


### raise {#raise}

**原型：**

```c
int raise(int sig);
```

`raise` 函数将给当前执行的线程或进程发送信号，如果信号处理函数已经被调用， `raise` 函数将等待信号处理函数调用结束才返回。


## 结语 {#结语}

信号处理函数是会被重复调用的，所以必要保存其是可重入的，注意处理逻辑。

另外本文中的代码都在 [signal](https://github.com/jouyouyun/examples/tree/master/signal) 中，这个 `repo` 也有其它的示例，有兴趣的可以看看。


## 附录 {#附录}


### 信号表 {#信号表}

```c
/* ISO C99 signals.  */
#define	SIGINT		2	/* Interactive attention signal.  */
#define	SIGILL		4	/* Illegal instruction.  */
#define	SIGABRT		6	/* Abnormal termination.  */
#define	SIGFPE		8	/* Erroneous arithmetic operation.  */
#define	SIGSEGV		11	/* Invalid access to storage.  */
#define	SIGTERM		15	/* Termination request.  */

/* Historical signals specified by POSIX. */
#define	SIGHUP		1	/* Hangup.  */
#define	SIGQUIT		3	/* Quit.  */
#define	SIGTRAP		5	/* Trace/breakpoint trap.  */
#define	SIGKILL		9	/* Killed.  */
#define SIGBUS		10	/* Bus error.  */
#define	SIGSYS		12	/* Bad system call.  */
#define	SIGPIPE		13	/* Broken pipe.  */
#define	SIGALRM		14	/* Alarm clock.  */

/* New(er) POSIX signals (1003.1-2008, 1003.1-2013).  */
#define	SIGURG		16	/* Urgent data is available at a socket.  */
#define	SIGSTOP		17	/* Stop, unblockable.  */
#define	SIGTSTP		18	/* Keyboard stop.  */
#define	SIGCONT		19	/* Continue.  */
#define	SIGCHLD		20	/* Child terminated or stopped.  */
#define	SIGTTIN		21	/* Background read from control terminal.  */
#define	SIGTTOU		22	/* Background write to control terminal.  */
#define	SIGPOLL		23	/* Pollable event occurred (System V).  */
#define	SIGXCPU		24	/* CPU time limit exceeded.  */
#define	SIGXFSZ		25	/* File size limit exceeded.  */
#define	SIGVTALRM	26	/* Virtual timer expired.  */
#define	SIGPROF		27	/* Profiling timer expired.  */
#define	SIGUSR1		30	/* User-defined signal 1.  */
#define	SIGUSR2		31	/* User-defined signal 2.  */

/* Nonstandard signals found in all modern POSIX systems
   (including both BSD and Linux).  */
#define	SIGWINCH	28	/* Window size change (4.3 BSD, Sun).  */

/* Archaic names for compatibility.  */
#define	SIGIO		SIGPOLL	/* I/O now possible (4.2 BSD).  */
#define	SIGIOT		SIGABRT	/* IOT instruction, abort() on a PDP-11.  */
#define	SIGCLD		SIGCHLD	/* Old System V name */

/* Not all systems support real-time signals.  bits/signum.h indicates
   that they are supported by overriding __SIGRTMAX to a value greater
   than __SIGRTMIN.  These constants give the kernel-level hard limits,
   but some real-time signals may be used internally by glibc.  Do not
   use these constants in application code; use SIGRTMIN and SIGRTMAX
   (defined in signal.h) instead.  */
#define __SIGRTMIN	32
#define __SIGRTMAX	__SIGRTMIN

/* Biggest signal number + 1 (including real-time signals).  */
#define _NSIG		(__SIGRTMAX + 1)
```

使用 `kill -l` 命令也可查看信号列表.


#### 可产生 `coredump` 的信号 {#可产生-coredump-的信号}

| ID | Name    | Description |
|----|---------|-------------|
| 3  | SIGQUIT | 终止进程    |
| 4  | SIGILL  | 非法指令    |
| 5  | SIGTRAP | 自陷        |
| 6  | SIGABRT | 异常终止    |
| 7  | SIGBUS  | 总线错误    |
| 8  | SIGFPE  | 浮点数异常  |
| 11 | SIGSEGV | 段错误      |
| 6  | SIGIOT  | I/O 自陷    |

`SIGIOT` 在 `PDP-11` 上产生 `LOT` 指令, 同 `SIGABRT` .
