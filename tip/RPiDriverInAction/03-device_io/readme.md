# 树莓派驱动开发实战03：设备IO访问技术

本文源码：https://github.com/Philon/rpi-drivers/tree/master/03-device_io

本文是上一篇《GPIO驱动之按键中断》的扩展，之前的文章侧重于中断原理及Linux/IRQ基础，用驱动实现了一个按键切换LED灯色的功能。但涉及中断的场景，就会拔出萝卜带出泥要实现IO的同步/异步访问方式。归根结底，驱动要站在(用户层)接口调用的角度，除了实现自身的功能，还要为用户层提供一套良好的访问机制。本文主要涉及以下知识点：

- 机制与策略原则
- IO阻塞/非阻塞——read/write
- IO多路复用——select/epoll
- 信号异步通知——signal

## 机制与策略

Linux的核心思想之一就是“一切皆文件”，而这其中最重要的原则便是“提供机制，而非策略”。文件——内核层与用户层分水岭，通过文件，用户可以用简单而统一的方式去访问复杂而多样的设备，且不必操心设备内部的具体细节。然而哪些部分该驱动实现，哪些部分又该留给用户实现，这是需要拿捏的，我个人理解：

- 机制，相当于怎么做，提供某个功能范围的实现形式、框架、标准
- 策略，相当于做什么，提供某种功能的具体实现方法和细节

以上一篇按键中断的驱动为例——“按一下切换一种灯色”，显然，这根本不符合驱动设计原则。首先，驱动把两种设备打包进一个程序；其次，驱动实现了“切换灯色”这个具体业务功能(策略)；最后，驱动根本没有提供用户层的访问机制。

还是回到需求——“按一下切换一种灯色”，理想情况下应该是这样的：
1. LED驱动——向用户层提供灯色切换机制
2. 按键驱动——向用户层提供“按下事件”通知/获取机制
3. 由用户层自行决定收到按键事件后，如何切换灯色

从以上分析看，原本一个驱动代码分拆分成led驱动、按键驱动、切灯app三个部分：led驱动已经在第1章《GPIO驱动之LED》里实现了，因此现在还差两件事：
- 按键驱动，砍掉原有的led功能实现，增加中断标志获取的访问机制
- 切灯app，这个就无所谓了，只要有了机制，策略想怎么写就怎么写

## IO的阻塞/非阻塞访问

阻塞/非阻塞，是访问IO的两种不同机制，即同步和异步获取。

所谓阻塞访问，就是当用户层`read/write`设备文件时，如果预期的结果还没准备好，进程就会被挂起睡眠，让出CPU资源，直到要读写的资源准备好后，重新唤醒进程执行操作。以本文实际的按键中断来说，当用户层读按键设备节点时，只要按键没有被按下，进程就应该一直阻塞在read函数，直到触发中断后才返回。

所谓非阻塞访问，就是调用`open(file, O_NONBLOCK)`或者`ioctl(fd, F_SETFL, O_NONBLOCK)`，将文件句柄设置为非阻塞模式，此时，如果要读写的资源还没准备好，`read/write`会立刻返回`-EAGAIN`错误码，进程不会被挂起。

简而言之：
- 阻塞模式：read/write时会被挂起，让出CPU，无法及时响应后续业务
- 非阻塞模式：read/write时不会挂起，占有CPU，不影响后续程序执行

显然，通过以上介绍，驱动需要在自己的`xxx_read() xxx_write()`等文件接口里实现阻塞功能，在Linux内核中主要通过“等待队列”来实现进程的阻塞与唤醒的。

### 等待队列

等待队列是基于链表实现的数据结构，用来表示某个事件上所有等待的进程。内核接口如下：

```c
#include <linux/wait.h>

// 等待队列链表数据结构
struct wait_queue_head {
	spinlock_t		    lock;
	struct list_head	head;
};
typedef struct wait_queue_head wait_queue_head_t;

// 初始化一个等待队列
#define init_waitqueue_head(wq_head)

// 声明一个等到节点(进程)
#define DECLARE_WAITQUEUE(name, tsk)

// 添加/删除节点到等待队列中
void add_wait_queue(struct wait_queue_head *wq_head, struct wait_queue_entry *wq_entry);
void remove_wait_queue(struct wait_queue_head *wq_head, struct wait_queue_entry *wq_entry);

///////////////////////////////////////////
// 往下看之前先记住两个函数
// set_current_state(value) 👈设置当前进程的状态，比如阻塞
// schedule()               👈调度其他进程
///////////////////////////////////////////

// 【接收】开始等待事件到来
// 以下宏，均是由上边的两个函数封装而言，不过是设置进程状态不同罢了
#define wait_event(wq_head, condition)                  // 不可被中断
#define wait_event_interruptible(wq_head, condition)    // 可被信号中断
#define wait_event_timeout(wq_head, condition, timeout) // 会超时
#define wait_event_interruptible_timeout(wq_head, condition, timeout) // 可被信号中断和超时

// 【发送】唤醒队列中的所有等待队列
#define wake_up(x)
#define wake_up_interruptible(x)
```

上边的API比较难理解的就是`wait_event_xxx`和`wake_up_xxx`，其实很简单，一般在驱动的读写接口里调用`wait_xxx`让进程切换到阻塞状态等待唤醒，然后在中断或其他地方调用`wake_up_xxx`即可唤醒队列。再有，通常情况下建议使用`interruptible`模式，否则进程将无法被系统信号中断。

### 最简单的按键阻塞实现

下面来实现按键的阻塞访问，当用`cat /dev/key`命令去读按键是否按下时，应该会被阻塞，直到按键真的被按下。为此，需要实现：
1. 在`init`函数中初始化gpio按键相关，以及一个“等待队列”
2. 在`read`函数中创建一个“等待”，然后进入阻塞，直到被唤醒
3. 在中断响应函数中，唤醒整个“等待队列”

PS：以下驱动也顺便实现了“非阻塞”访问模式，这个其实很简单，无非就是判断以下文件标识是否为`O_NONBLOCK`即可。

```c
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>

#define KEY_GPIO  17

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Philon | https://ixx.life");

static wait_queue_head_t r_wait;

// 读取阻塞
static ssize_t gpiokey_read(struct file *filp, char __user *buf, size_t len, loff_t * off)
{
  int data = 1;
  // 新建一个”等待“并加入队列
  DECLARE_WAITQUEUE(wait, current);
  add_wait_queue(&r_wait, &wait);

  if ((filp->f_flags & O_NONBLOCK) && !gpio_get_value(KEY_GPIO)) {
    // 如果是非阻塞访问，且按键没有按下时，直接返回错误
    return -EAGAIN;
  }

  // 进程进入阻塞状态，等待事件唤醒(可被信号中断)
  wait_event_interruptible(r_wait, gpio_get_value(KEY_GPIO));
  remove_wait_queue(&r_wait, &wait);

  // 被唤醒后，进行业务处理，返回一个“按下”标志给用户进程
  len = sizeof(data);
  if ((data = copy_to_user(buf, &data, len)) < 0) {
    return -EFAULT;
  }
  *off += len;

  return 0;
}

static int press_irq = 0;
static struct timer_list delay;

// 按键中断顶半部响应及防抖延时判断
static irqreturn_t on_key_pressed(int irq, void* dev)
{
  mod_timer(&delay, jiffies + (HZ/20));
  return IRQ_HANDLED;
}
static void on_delay50(struct timer_list* timer)
{
  if (gpio_get_value(KEY_GPIO)) {
    // 按下按键后，唤醒阻塞队列
    wake_up_interruptible(&r_wait);
  }
}

struct file_operations fops = {
  .owner = THIS_MODULE,
  .read = gpiokey_read,
};

struct miscdevice gpiokey = {
  .minor = 1,
  .name = "gpiokey",
  .fops = &fops,
  .nodename = "mykey",
  .mode = 0700,
};

static int __init gpiokey_init(void)
{
  // 初始化定时器，用于防抖延时
  timer_setup(&delay, on_delay50, 0);
  add_timer(&delay);

  // 初始化“读阻塞”等待队列
  init_waitqueue_head(&r_wait);

  // 向内核申请GPIO和IRQ并绑定中断处理函数
  gpio_request_one(KEY_GPIO, GPIOF_IN, "key");
  press_irq = gpio_to_irq(KEY_GPIO);
  if (request_irq(press_irq, on_key_pressed, IRQF_TRIGGER_RISING, "onKeyPress", NULL) < 0) {
    printk(KERN_ERR "Failed to request irq for gpio%d\n", KEY_GPIO);
  }

  // 注册驱动模块并创建设备节点
  misc_register(&gpiokey);
  return 0;
}
module_init(gpiokey_init);

static void __exit gpiokey_exit(void)
{
  misc_deregister(&gpiokey);
  free_irq(0, NULL);
  gpio_free(KEY_GPIO);
  del_timer(&delay);
}
module_exit(gpiokey_exit);
```

![按键中断阻塞访问效果](https://i.loli.net/2019/08/18/iX8BPOrq1pw74aH.gif)

## 多路复用IO模型-poll

如果程序只监听一个设备，那用阻塞或非阻塞足够了，但如果设备数量繁多呢？比如我们的键盘，有一百多个键，难道每次都要全部扫描一遍有没有被按下？(当然，键盘事件有另外的机制，这里只是举个例子)

这个时候轮询操作就非常有用了，据我所知有很多小型的网络服务正是用此机制实现的高性能并发访问，简单来说，就是把成千上万个socket句柄放到一种名叫`fd_set`的集合里，然后通过`select()/epoll()`同时监听集合里的句柄状态，其中任何一个socket可读写时就唤醒进程并及时响应。

综上，设备驱动要做的，便是实现`select/epoll`的底层接口。而有关select的应用层开发这里就不介绍了，网上一大堆。

驱动模块的多路复用实现其实非常简单，和读写接口一样，你只需实现`file_operations`里的poll接口：
```c
#include <linux/poll.h>

// file_operations->poll
// 由驱动自行实现多路复用功能
__poll_t (*poll) (struct file *, struct poll_table_struct *);

// 在具体实现poll接口是需要调用
// 该函数本身不会引发阻塞，仅仅是把select的等待指向驱动模块
// 睡眠是由用户层等select()自身完成的
// 当它遍历完全部的设备文件后，相当于把自己的等待节点指向了每一个设备驱动
// 任何一个设备唤醒时都会触发select唤醒
void poll_wait(struct file * filp, wait_queue_head_t * wait_address, poll_table *p);

// 既然poll接口不会阻塞，那就直接告诉用户层，设备当前的可操作状态
// 便于select判断文件的读写状态
#define POLLIN		0x0001  // 可读
#define POLLPRI		0x0002  // 紧急数据可读
#define POLLOUT		0x0004  // 可写
#define POLLERR		0x0008  // 错误
#define POLLHUP		0x0010  // 被挂起
#define POLLNVAL	0x0020  // 非法

#define POLLRDNORM	0x0040  // 普通数据可读
#define POLLRDBAND	0x0080  // 优先数据可读
#define POLLWRNORM	0x0100  // 普通数据可写
#define POLLWRBAND	0x0200  // 优先数据可写
#define POLLMSG     0x0400  // 有消息
#define POLLREMOVE	0x1000  // 被移除
#define POLLRDHUP   0x2000  // 读被挂起
```

结合第二小结的等待队列，实现gpio按键的多路复用IO就非常简单了，只需要在原有的代码里加入下面这段：

```c
// 实现内核的poll接口
static __poll_t gpiokey_poll(struct file *filp, struct poll_table_struct *wait)
{
  __poll_t mask = 0;  // 设备的可操作状态
  
  // 加入等待队列
  poll_wait(filp, &r_wait, wait);
  if (gpio_get_value(KEY_GPIO)) {
    // 按键设备不存在写，所以总是返回可读，如果可以时
    mask = POLLIN | POLLRDNORM;
  }

  return mask;
}

// 注意接口要加入到文件操作描述里
struct file_operations fops = {
  .owner = THIS_MODULE,
  .read = gpiokey_read,
  .poll = gpiokey_poll,
};
```

## 异步通知-信号

不论IO阻塞、非阻塞还是多路复用，都是由应用程序主动向设备发起访问，有没有一种机制，就像邮箱一样，当有信息来临时再通知用户，用户仅仅是被动接收——答：信号！

信号本质上来说，就是软件层对中断的一种模拟。正如常见的`Ctrl+C`、`kill`等，都是向进程发送信号的手段。所以信号也可以理解为是一种特殊的中断号或事件ID。其实在Linux应用开发中，会涉及很多的“终止/定时/异常/掉电”等信号捕获，我们写的程序之所以能被`Ctrl+C`终止，就是因为在应用接口里已经实现了相关信号的捕获处理。

为了更好地理解设备驱动有关信号机制的实现，必须先站在用户层的角度看看信号是如何被调用的。有关Linux常用的标准信号这里也不展开讨论，请用好互联网。这里仅仅是看一个应用程序如何接收指定设备的`SIGIO`信号的：

```c
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/fcntl.h>

// 按键信号的处理函数
void on_keypress(int sig)
{
  printf("key pressed!!\n");
}

int main(int argc, char* argv[])
{
  // 第一步：将驱动的拥有者指向本进程，否则设备信号不知道发给谁
  int oflags = 0;
  int fd = open("/dev/mykey", O_RDONLY);
  fcntl(fd, F_SETOWN, getpid());
  oflags = fcntl(fd, F_GETFL) | O_ASYNC;
  fcntl(fd, F_SETFL, oflags);

  // 第二步：捕获想要的信号，并绑定到相关处理函数
  signal(SIGIO, on_keypress);

  // 以下无关紧要，就是等到程序退出
  printf("I'm doing something ...\n");
  getchar();
  close(fd);
  return 0;
}
```

从以上代码来看，应用程序要实现信号捕获需要操作：

1. 用`F_SETOWN`让设备文件指向自己，确保信号的传输目的地
2. 用`O_ASYNC`或者`FASYNC`标志告诉驱动(即调用驱动的`xxx_fasync`接口)，我要去做其他事了，有情况请主动通知我
3. 设置信号捕获及相关处理handler

所以对应的，内核模块也需要实现信号的发送也需要三个步骤：

1. `filp->f_onwer`指向进程ID，这点已经又内核完成，不用再实现
2. 实现`xxx_fasync()`接口，在里面初始化一个`fasync_struct`用于信号处理
3. 当有情况时，使用`kill_fasync()`发送信号给进程

内核具体接口如下：

```c
// 设备文件的异步接口，当用户层标记了O_ASYNC或FASYNC时触发
struct file_operations {
  int (*fasync) (int fd, struct file *filp, int mode);
  ...
};

// 异步“小助手”，初始化用，一般在xxx_fasync()接口里调用
// 前面三个参数由用户层传进来，最后一个是“异步队列”，该函数会为其分配内存初始化
int fasync_helper(int fd, struct file *filp, int mode, struct fasync_struct **fa);

// 通过fa，发送信号到进程
// 后两个参数为信号ID、可读/可写状态
void kill_fasync(struct fasync_struct **fa, int sig, int band);

// 最后务必注意，在xxx_close或xxx_release中让文件描述从异步队列中剥离
// 否则用户进程挂了，驱动还一直向其发送信号，岂不有病
static int xxx_close(struct inode *node, struct file *filp)
{
  ...
  xxx_fasync(-1, filp, 0);
}
```

搞清楚了内核关于异步信号的机制，下面用让gpiokey支持SIGIO信号吧！

```c
static struct {
  int irq;                  // 按键GPIO中断号
  struct timer_list delay;  // 防抖延时
  wait_queue_head_t r_wait; // IO阻塞等待队列
  struct fasync_struct* fa; // 异步描述
} dev;

// 实现fops->gpiokey_fasync接口，支持用户层的FASYNC标记
// 将用户进程文件描述添加到异步队列中
static int gpiokey_fasync(int fd, struct file *filp, int mode)
{
  return fasync_helper(fd, filp, mode, &dev.fa);
}

// 用户进程关闭设备时，务必将其从异步队列中剥离
static int gpiokey_close(struct inode *node, struct file *filp)
{
  gpiokey_fasync(-1, filp, 0);
  return 0;
}

// 当按键中断触发后，将信号发送至用户进程
static irqreturn_t on_key_pressed(int irq, void* dev_id)
{
  mod_timer(&dev.delay, jiffies + (HZ/20));
  return IRQ_HANDLED;
}
static void on_delay50(struct timer_list* timer)
{
  if (gpio_get_value(KEY_GPIO)) {
    wake_up_interruptible(&dev.r_wait);   // 唤醒阻塞队列
    kill_fasync(&dev.fa, SIGIO, POLL_IN); // 发送SIGIO异步信号
  }
}

struct file_operations fops = {
  ...
  .fasync = gpiokey_fasync,
  .release = gpiokey_close,
};
```

从输出结果中可以看到，程序启动并执行后续，完全没有监听设备，当按键被按下时，信号传回进程并触发了`on_keypress()`函数。

```sh
philon@rpi:~/modules $ sudo insmod gpiokey.ko
philon@rpi:~/modules $ ./signal_test 
I'm doing something ...
key pressed!!
key pressed!!
```

## 异步IO

自Linux2.6以后，IO的异步访问又多了一种新方式——`aio`，此方式在实际开发中并不多见，尤其是嵌入式领域！因此本文不打算深入讨论，这里作为知识扩展仅做个简单介绍。

异步IO的核心思想就是——回调，例如`aio_read(struct aiocb *cb)`和`aio_write(truct aiocb *cb)`，程序调用该函数后不会阻塞，当文件读写就绪后，会自动根据`cb`描述进行回调。

此外，AIO有应用层基于线程的glibc实现，以及内核层的fops接口实现，甚至还有类型libuv、libevent这样的事件驱动的第三方框架可供使用。

就我个人而言，技术是把双刃剑，回调是一种看似美妙的骚操作，但如果你编写的业务具有强逻辑性，那回调在时序上的失控，以及返回状态的多样化，会随着代码的壮大而进入回调陷阱，深深地无法自拔。给我这种感受的并非C语言，而是JavaScript。

总之，没有最优秀的技术，只有最适用的场景！我的原则是：用回调，远离嵌套回调。

## 小结

- Linux内核模块应当“提供机制，而非策略”
- 阻塞IO是在用户层读写访问时，是进程睡眠，由驱动来唤醒
- 非阻塞IO是有`IO_NONBLOCK`标记时，当资源不可访问时，直接返回`-EAGAIN`
- 多路复用IO是通过`select/epoll`进行多个设备监听，驱动须实现对应的`fops->poll`接口
- 异步IO即信号，由设备驱动作为信号源，主动向进程发送通知
- 不同的IO同步/异步访问机制无优劣之分，而是取决于具体的应用场景
- 务必搞懂`等待队列`，它贯穿以上几种IO访问机制