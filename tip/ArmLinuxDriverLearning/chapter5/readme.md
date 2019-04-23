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

## 监视 - oops

## 查询 - proc

## 调试器 - kdb