# Linux驱动调试技术

古人云:别跟老子说什么断点、跟踪、gdb、lldb...老子调代码从来都是print😂️。调试在程序开发中是非常重要的手段，就像单元测试一样，是保障软件质量的主要手段之一，别不当回事！

言归正传，linux内核提供了多种调试技术，但因为驱动程序不是普通的程序，很多常见的调试工具到内核这一层基本都扑街了，printk反而成了最朴实有效的手段之一，但不论如何，多掌握其他的调试手段和工具，对于今后定位内核模块的错误，总会有帮助的。

## 打印 - printk

`printk`就是常规的打印输出，但与应用成的`printf`稍微不同，往往会看到这样的调用`printk(KERN_ALERT"hello world")`，其中的`KERN_ALERT`表示打印级别，内核源码中定义了多种打印级别，且看定义：
```c
#define KERN_SOH      "\001"       /* ASCII Start Of Header */

#define KERN_EMERG    KERN_SOH "0" /* 紧急事件消息,在系统崩溃前提示用的 */
#define KERN_ALERT    KERN_SOH "1" /* 用于需要立即采取动作的情况 */
#define KERN_CRIT     KERN_SOH "2" /* 临界状态，涉及严重的硬件或软件操作失败时提示 */
#define KERN_ERR      KERN_SOH "3" /* 错误报告 */
#define KERN_WARNING  KERN_SOH "4" /* 常规警告 */
#define KERN_NOTICE   KERN_SOH "5" /* 普通提示，常见与安全相关的汇报 */
#define KERN_INFO     KERN_SOH "6" /* 提示性信息，比如硬件信息 */
#define KERN_DEBUG    KERN_SOH "7" /* 调试信息 */
```

从上定义可以看到，内核共提供了0-7个级别，**数值越小，优先级越高**。

### 屏蔽其他级别打印

`/proc/sys/kernel/printk`文件很重要，可以通过它来屏蔽不同级别的打印输出，我们迅速写一段代码：

```c
static int __init meme_init(void)
{
    printk(KERN_EMERG"emerg 0\n");
    printk(KERN_ALERT"alert 1\n");
    printk(KERN_CRIT"crit 2\n");
    printk(KERN_ERR"err 3\n");
    printk(KERN_WARNING"warning 4\n");
    printk(KERN_NOTICE"notice 5\n");
    printk(KERN_INFO"info 6\n");
    printk(KERN_DEBUG"debug 7\n");

    return 0;
}
module_init(meme_init);
```

以上代码只是在模块加载的时候打印了7个级别的内容，现在做一件事：

```sh
# 查看该文件发现默认打印级别小于7，即除了KERN_DEBUG级别，其他都能显示到终端
/ # cat /proc/sys/kernel/printk
7	4	1	7

# 现在强制打印级别小于3，即KERN_ERR及其之后级别的内容不再显示
/ # echo 3 > /proc/sys/kernel/printk

# 加载模块
/ # insmod meme.ko 
emerg 0
alert 1
crit 2
```

从上边的指令可以看到，通过修改/proc/sys/kernel/printk文件的值可以直接强制控制台打印的日志级别，这可要比你反复注释+编译的手段高明多了。

## 通过procfs文件查询调试

printk函数固然简单易用，但除了逼格很low之外还存在个技术上的障碍——大量使用printk会极大地拖累程序的性能，原则上也仅用于常规和错误信息提示，像for、while之类的循环内千万别用它。

然而实际上用户需要时刻掌握各种设备的状态信息，比如cpu当前频率/温度、内存占用率等等，printk显然不能胜任，轮到procfs登场了，我们知道proc是内核的一个虚拟文件系统，能够以文件的形式展现整个系统内部的信息，而**通过procfs文件查询调试**技术，就是在`/proc`目录下创建驱动模块自己的目录和文件——俗称入口，供用户随时访问。

我个人觉得，驱动暴露在proc文件系统的入口其实很少会用于“调试”，而是一些关键信息的展示，比如`cpuinfo`、`meminfo`等文件，这就是典型的应用场景，cpu和内存驱动创建的文件，可以直接查看相关设备的细节信息。

在`proc`文件系统中创建入口文件非常简单，总共有两种形式：

### 1. 普通proc入口

之前的学习笔记中，我们已经创建了meme驱动模块的设备节点`/dev/meme`，现在实现一个新功能，用户只需要通过命令`cat /proc/meme/state`即可查看设备节点`/dev/meme`的文件访问状态，例如打开、关闭、正在读、正在写等等。

首先，在procfs创建/删除入口文件的API有这么几个：
```c
// proc入口文件的结构体定义
struct proc_dir_entry;

// 在procfs下创建一个目录
// @name    要创建的目录名
// @parent  上级目录，NULL表示/proc
struct proc_dir_entry *proc_mkdir(const char *name, struct proc_dir_entry *parent);

// 在procfs下创建一个文件
// @name    要创建的文件名
// @mode    文件的访问权限
// @parent  所在目录，NULL表示/proc
// @proc_fops 文件操作结构，与字符设备一样的机制
struct proc_dir_entry *proc_create(const char *name, umode_t mode, struct proc_dir_entry *parent, const struct file_operations *proc_fops);

// 在procfs下删除一个文件或目录
// @name    要删除的入口名
// @parent  所在上级目录
void remove_proc_entry(const char *name, struct proc_dir_entry *parent);
```

然后，通过调用`proc_mkdir()`和`proc_create()`即可创建出`/proc/meme/state`目录和文件了，几乎没什么难度，注意在创建proc的入口文件时需要指定一个`file_operations *fops`，其实就是文件访问函数的映射，和字符设备章节中的操作一模一样。而`/proc/meme/state`是一个只读状态文件，所以我们只需要实现`read`函数的功能——返回状态字符串。

```c
/**
 * meme_proc.c
 */

#include <linux/proc_fs.h>

// 全局变量，meme的设备节点状态
int meme_cdev_state = 0;

static ssize_t meme_proc_read(struct file* filp, char __user* buf, size_t len, loff_t* off)
{
    int rc = 0;
    const char* state = NULL;

    if (*off > 0) {
        return 0;
    }

    switch (meme_cdev_state) {
        case MEME_STATE_OPENED: state = "opened"; break;
        case MEME_STATE_CLOSED: state = "closed"; break;
        case MEME_STATE_READING: state = "reading"; break;
        case MEME_STATE_WRITING: state = "writing"; break;
        default: state = "unknown"; break;
    }

    len = strlen(state);
    if ((rc = copy_to_user(buf, state, len)) < 0) {
        return rc;
    }
    buf[len++] = '\n';
    buf[len++] = '\0';
    return len;
}

// 绑定用户层read时的触发函数
const struct file_operations proc_fops = {
    .owner = THIS_MODULE,
    .read = meme_proc_read,
};

// procfs入口目录
struct proc_dir_entry* meme_proc_entry = NULL;

int __init meme_proc_init(void)
{
    // 即/proc/meme/state，用于记录设备描述符/dev/meme的文件访问状态
    struct proc_dir_entry* meme_state_file = NULL;

    // 创建meme模块的proc目录，即/proc/meme/
    meme_proc_entry = proc_mkdir("meme", NULL);
    if (meme_proc_entry == NULL) {
        return -EINVAL;
    }

    // 创建/proc/meme/state文件，且该文件为只读
    meme_state_file = proc_create("state", 0500, meme_proc_entry, &proc_fops);
    if (meme_state_file == NULL) {
        return -EINVAL;
    }

    return 0;
}

void __exit meme_proc_exit(void)
{
    // 卸载模块时，自动删除所有/proc/meme所创建的文件及目录
    remove_proc_entry("state", meme_proc_entry);
    remove_proc_entry("meme", NULL);
}
```

以上是meme驱动模块在proc入口状态文件的相关实现，既然要监听的是`/dev/meme`设备节点的访问状态，上述代码是通过`meme_cdev_state`全局变量来实现的，故这边变量自然是在字符设备的代码实现中被修改的。

```c
/**
 * main.c
 */

static ssize_t meme_read(struct file* filp, char __user *buf, size_t size, loff_t* off)
{
    meme_cdev_state = MEME_STATE_READING;
    ...
}

static ssize_t meme_write(struct file* filp, const char __user *buf, size_t size, loff_t* off)
{
    meme_cdev_state = MEME_STATE_WRITING;
    ...
}

static int meme_open(struct inode* inode, struct file* filp)
{
    meme_cdev_state = MEME_STATE_OPENED;
    ...
}

static int meme_close(struct inode* inode, struct file* filp)
{
    meme_cdev_state = MEME_STATE_CLOSED;
    ...
}
```

如此一来，用户访问`/dev/meme`设备节点，`meme_cdev_state`值就会被改变，对应的状态自然就暴露到`/proc/meme/state`的read触发函数中了，完美～

测试一下：

```sh
/ # insmod meme.ko 
meme: loading out-of-tree module taints kernel.
meme init: 250:0
/ # mknod /dev/meme c 250 0
/ # while true; do echo 123 >> /dev/meme; done &
/ # cat /proc/meme/state 
opened
/ # cat /proc/meme/state 
closed
/ # cat /proc/meme/state 
opened
/ # cat /proc/meme/state 
writing
```

### 2. seq_file接口

`seq_file`主要是用于处理那些比较“大”的proc入口文件。所谓的大不是说文件体积，举个例子，当我们需要开发一个串口驱动，而这个驱动需要记录串口每一次的收发历史记录，用这种方式再何时不过。seq是序列的意思，即通过迭代的方式，把驱动的某些状态信息按顺序打印出来。

`seq_file`提供了4个迭代函数：
```c
struct seq_operations {
    // 首次访问时触发
	void * (*start) (struct seq_file *m, loff_t *pos);
    // 结束访问时触发
	void (*stop) (struct seq_file *m, void *v);
    // 迭代访问时触发
	void * (*next) (struct seq_file *m, void *v, loff_t *pos);
    // 打印展示时触发
	int (*show) (struct seq_file *m, void *v);
};
```

现在再创建一个入口文件`/proc/meme/info`，当用户通过命令`cat /proc/meme/info`时，打印0-100行计数内容：

```c
/**
 * meme_seq_file.c
 */

#include <linux/seq_file.h>

#define MAXNUM 100

// 访问文件时首先调用的接口
static void* meme_seq_start(struct seq_file* m, loff_t* pos)
{
    int* v = NULL;
    if (*pos < MAXNUM) {
        v = kmalloc(sizeof(int), GFP_KERNEL);
        *v = *pos;
        seq_printf(m, "start: *(%p) = %d\n", v, *(int*)v);
    }

    // start函数返回NULL表示pos已到达文件末尾
    return v;
}

// 每次迭代时调用，其中v是之前一次迭代(start或next)的返回值
static void* meme_seq_next(struct seq_file* m, void* v, loff_t* pos)
{
    int num = *(int*)v;
    if (num++ >= MAXNUM) {
        // 返回NULL停止迭代
        kfree(v);
        return NULL;
    }

    // 每次迭代，v和文件游标都增加1
    *(int*)v = *pos = num;
    return v;
}

// 结束迭代时调用，如果在start中有内存分配，应该在这里进行内存清理
// 但由于next的最后一次迭代肯定返回NULL，所以这里的v地址一定为NULL
// 不需要作任何处理
static void meme_seq_stop(struct seq_file* m, void* v)
{

}

// 展示时调用，主要是将v的内容格式化并输出到用户空间
static int meme_seq_show(struct seq_file* m, void* v)
{
    seq_printf(m, "show: *(%p) = %d\n", v, *(int*)v);
    return 0;
}

// 映射seq的start、next、stop、show四个迭代器的函数
const struct seq_operations meme_seq_ops = {
    .start = meme_seq_start,
    .next = meme_seq_next,
    .stop = meme_seq_stop,
    .show = meme_seq_show,
};

static int meme_seq_open(struct inode* inode, struct file* filp)
{
    // 绑定迭代操作的4个函数到/proc/meme/info文件
    return seq_open(filp, &meme_seq_ops);
}

// /proc/meme/info的文件操作映射，除了open需要自己实现外，其他均使用内部定义好的
static const struct file_operations meme_seq_fops = {
    .owner = THIS_MODULE,
    .open = meme_seq_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = seq_release,
};

int __init meme_seq_init(void)
{
    // 创建seq入口文件，即/proc/meme/info，并绑定seq相关文件函数
    proc_create("info", 0500, meme_proc_entry, &meme_seq_fops);
    return 0;
}

void __exit meme_seq_exit(void)
{
    // 删除/proc/meme/info文件
    remove_proc_entry("info", meme_proc_entry);
}
```

从上边的代码可以看到，除了实现4个迭代的触发函数之外，还需要实现文件的open触发，剩下的内容就和常规的proc入口文件创建机制没什么区别了。测试一下：

```sh
/ # insmod meme.ko
/ # cat /proc/meme/info
start: *(ada2e682) = 0
show: *(ada2e682) = 0
show: *(ada2e682) = 1
show: *(ada2e682) = 2
show: *(ada2e682) = 3
...
show: *(ada2e682) = 99
show: *(ada2e682) = 100
```

代码实现的比较简单，在`start`函数中为迭代分配一段内容空间用于计数，而在`next`每次迭代时将计数累加1直到100后跳出迭代，每次迭代后都会自动调用`show`将记录值打印出来。

## 通过strace命令监视

### strace移植

由于Busybox默认是不带strace的，需要自行移植。
```sh
# 下载strace源码并移植
$ git clone https://github.com/strace/strace.git
$ cd strace && ./bootstrap
$ mkdir build && cd build
$ ../configure prefix=$(pwd)/install --host=arm-linux-gnueabihf CC=arm-linux-gnueabihf-gcc
$ make -j8 && make install

# 将strace命令拷贝至目标文件系统
$ sudo mount -o loop ../../rootfs.ext3 /mnt/
$ sudo cp install/bin/strace /mnt/usr/bin
$ sudo umount /mnt
```

参照官方文档测试一下：
```sh
/ # strace -c ls > /dev/null 
% time     seconds  usecs/call     calls    errors syscall
------ ----------- ----------- --------- --------- ----------------
 33.51    0.010491         201        52        48 openat
 24.14    0.007557         157        48        47 stat64
 14.51    0.004544         378        12           lstat64
  5.88    0.001842         921         2           getdents64
  5.58    0.001748         194         9           mmap2
  4.45    0.001393         154         9           read
  3.58    0.001120         140         8           mprotect
  2.33    0.000730         121         6           _llseek
  1.87    0.000586         117         5           fstat64
  1.59    0.000498         124         4           close
  1.19    0.000373          93         4         3 ioctl
  0.58    0.000181          60         3           brk
  0.46    0.000144         144         1           set_tls
  0.32    0.000099          99         1           write
  0.00    0.000000           0         1           execve
  0.00    0.000000           0         2         2 access
  0.00    0.000000           0         1           gettimeofday
  0.00    0.000000           0         1           uname
  0.00    0.000000           0         1           getuid32
------ ----------- ----------- --------- --------- ----------------
100.00    0.031306                   170       100 total
```

又或者测试一下meme驱动模块
```sh
/ # strace echo 123 > /dev/meme
...
...
#最后几行
close(3)                                = 0
set_tls(0x76fbe4f0)                     = 0
mprotect(0x76ef0000, 8192, PROT_READ)   = 0
mprotect(0x76f12000, 4096, PROT_READ)   = 0
mprotect(0x76f95000, 4096, PROT_READ)   = 0
mprotect(0x533000, 12288, PROT_READ)    = 0
mprotect(0x76fbf000, 4096, PROT_READ)   = 0
getuid32()                              = 0
brk(NULL)                               = 0x537000
brk(0x558000)                           = 0x558000
write(1, "123\n", 4)                    = 4
exit_group(0)                           = ?
+++ exited with 0 +++
```

总之`strace`是一个非常牛逼的诊断工具，由于本文主要针对的是Linux驱动模块技术，关于这个命令的使用详解就不赘述了，自行Google。

## oops消息

oops也就是内核甩了一跤所发出的惨叫，当然导致内核摔跤的绊脚石大概率是我们写出了质量底下的驱动模块导致。举个最简单的例子：
```c
static void __init meme_init(void)
{
    *(int*)NULL = 123;
}
module_init(meme_init)
```

以上是meme模块加载时的初始化代码，但是我们试图往`NULL`地址里赋值，很显然这将引发段错误，如果直接insmod模块，不出意外内核将发出一声惨叫——oops!!!

来看看效果：
```sh
/ # insmod meme.ko

meme: loading out-of-tree module taints kernel.
Unable to handle kernel NULL pointer dereference at virtual address 00000000
pgd = 7dfd5e30
[00000000] *pgd=00000000
Internal error: Oops: 805 [#1] SMP ARM
Modules linked in: meme(O+)
CPU: 0 PID: 832 Comm: insmod Tainted: G           O      5.0.7 #1
Hardware name: ARM-Versatile Express
PC is at meme_init+0x18/0x94 [meme]
LR is at do_one_initcall+0x54/0x1fc
pc : [<7f005018>]    lr : [<80102e70>]    psr: 600f0013
sp : 9ddfbdc0  ip : 9dce1540  fp : 80b08c08
r10: 00000000  r9 : 7f002040  r8 : 7f005000
r7 : 00000000  r6 : ffffe000  r5 : 7f002240  r4 : 00000000
r3 : 0000007b  r2 : 7ed12b89  r1 : 00000000  r0 : 00000000
Flags: nZCv  IRQs on  FIQs on  Mode SVC_32  ISA ARM  Segment none
Control: 10c5387d  Table: 7de00059  DAC: 00000051
Process insmod (pid: 832, stack limit = 0x560ea799)
Stack: (0x9ddfbdc0 to 0x9ddfc000)
bdc0: 80b08c08 80b603c0 ffffe000 80102e70 00000000 80136cc8 006000c0 00000000
bde0: 00000000 9ddfbde4 9ddfbde4 7ed12b89 7f002088 a9245000 a9244fff fffff000
be00: 00000000 9dce1700 9dce1700 80b76b90 00000001 9dce1a24 7f002040 7ed12b89
be20: 7f002040 00000001 9dce1a00 9dce14c0 9dce1a24 801a03b8 9dce1a24 80232fc0
be40: 9ddfbf30 9ddfbf30 00000001 9dce1a00 00000001 8019f5c0 7f00204c 00007fff
be60: 7f002040 8019ca04 00000001 7f002088 8019c364 7f002154 7f002170 8094b580
be80: 7f00222c 7f006000 808e1fd8 808e1fe4 808e203c 80b08c08 9dce7600 fffff000
bea0: 80b0b5c4 006002c0 9dce7600 00000043 00000000 00000000 00000000 00000000
bec0: 00000000 00000000 6e72656b 00006c65 00000000 00000000 00000000 00000000
bee0: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
bf00: 00000000 7ed12b89 00000080 00002a94 76d4da9c a9243a94 ffffe000 80b08c08
bf20: 004cb9a7 00000000 00000051 8019fbf8 a92018d2 a9201a40 a9201000 00042a94
bf40: a9243314 a924313c a9234104 00003000 00003180 00000000 00000000 00000000
bf60: 00001d50 0000002d 0000002e 00000014 00000000 00000010 00000000 7ed12b89
bf80: 9e61a600 76ee7404 00042a94 76f0ddc0 00000080 80101204 9ddfa000 00000080
bfa0: 004b78e3 80101000 76ee7404 00042a94 76d0b008 00042a94 004cb9a7 76f0f968
bfc0: 76ee7404 00042a94 76f0ddc0 00000080 00000001 7ec25eac 004cb9a7 004b78e3
bfe0: 7ec25b68 7ec25b58 0042b291 76de3f12 200f0030 76d0b008 00000000 00000000
[<7f005018>] (meme_init [meme]) from [<80102e70>] (do_one_initcall+0x54/0x1fc)
[<80102e70>] (do_one_initcall) from [<801a03b8>] (do_init_module+0x64/0x1f4)
[<801a03b8>] (do_init_module) from [<8019f5c0>] (load_module+0x1ed4/0x23b4)
[<8019f5c0>] (load_module) from [<8019fbf8>] (sys_init_module+0x158/0x18c)
[<8019fbf8>] (sys_init_module) from [<80101000>] (ret_fast_syscall+0x0/0x54)
Exception stack(0x9ddfbfa8 to 0x9ddfbff0)
bfa0:                   76ee7404 00042a94 76d0b008 00042a94 004cb9a7 76f0f968
bfc0: 76ee7404 00042a94 76f0ddc0 00000080 00000001 7ec25eac 004cb9a7 004b78e3
bfe0: 7ec25b68 7ec25b58 0042b291 76de3f12
Code: e3475f00 e3a04000 e3a0307b e1a01004 (e5843000) 
---[ end trace c5673f359b9dfbf8 ]---
Segmentation fault
```

多么熟悉的味道，信息量好大，让人眼花缭乱。别着急，主要留意几个地方：
1. `Modules linked in: meme(O+)`说明导致oops的是meme模块驱动。
2. `PC is at meme_init+0x18/0x94 [meme]`说明引发oops的函数是`meme_init`。
3. `Exception stack(0x9ddfbfa8 to 0x9ddfbff0)`以及之后的N个地址表示异常的栈区，读懂这些地址需要比较丰富的经验，刚开始没必要太纠结这部分。
4. `Segmentation fault`表示错误类型是段错误

## 使用gdb、kgdb等调试器

关于gdb调试神器如何应用到内核模块，就不详述了，如果真想了解可以参考IBM这篇[《Linux系统内核的调试》](https://www.ibm.com/developerworks/cn/linux/l-kdb/index.html)。

其实gdb可以提供诸如断点、变量监视、单步执行等非常有用的功能，在追踪bug的效率方面无疑可以碾压上述的几种方式。但是可别忘了，我们写的是Linux内核模块，这些断点、单步执行的功能其实很难应用的内核层，而且驱动本质上是为硬件服务的，你要代码单步执行可以，硬件那边可没有暂停键啊！

所以，在Linux驱动模块的开发过程中，gdb这种工具反而应该尽量避免！

## 小结一下

- `printk()`是最朴实有效的调试方式，Linux一共提供了8个打印级别，可以通过`echo N > /proc/sys/kernel/printk`来限制级别。
- 可以通过创建`/proc/<entry>`入口文件来减少对printk的依赖
- `seq_file`是procfs入口文件的特殊实现方式，主要用于状态信息庞大的驱动，按顺序迭代输出。
- `strace`是一个非常有用的程序执行跟踪的命令。
- `oops`本质上是内核某些地方执行出错产生的提示信息，可以用于定位问题根源。