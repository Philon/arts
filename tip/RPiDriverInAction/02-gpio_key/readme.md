# 树莓派驱动开发实战02：GPIO驱动之按键中断

本文源码：https://github.com/Philon/rpi-drivers/tree/master/02-gpio_key

在上一篇中主要学习了GPIO原理及Linux字符设备，其过程大致是这样子的：

```
电路图“看引脚” --> 手册“看物理地址” --> 寄存器手册“看GPIO逻辑控制” --> 复用功能 --> 地址操作
```

可以看到，整个过程是非常繁琐的，驱动程序必须精确到处理器的每一根引脚的状态，如果所有驱动都这么写估计当场就跪了。因此，本文除了学习GPIO中断原理之外，更重要是掌握以下知识：

- 混杂设备机制
- Linux内核GPIO接口
- ARM中断基础
- Linux内核中断接口

# ARM中断基础

中断，就是由外部电路产生的一个电信号，强制CPU从当前执行代码区转移到中断处理函数。ARM架构的CPU中断硬件原理和这个差不多，注意这里说的是ARM的CPU，仅仅代表处理器当中的一个核，不要把CPU和SoC划等号。

CPU能提供的中断资源是非常有限的，一般也就一两个“引脚”，但我们在看芯片手册的时候就会发现，几乎每个GPIO都具备中断功能，那可是几十上百个中断啊！这归功于内部继承的PIC——可编程中断控制器，它负责监听所有GPIO的中断信号，并在外设给出中断信号时真正去触发CPU中断，并告诉CPU是谁触发的。这种中断信号源被抽象为——中断号。

在现代多核处理器架构下，ARM用的是GIC(通用中断控制器)，它能支持SGI(软件生成中断)、PPI(单核私有外设中断)、SPI(多核共享外设中断)。默认情况下，ARM处理器的外设中断总是先给到CPU0，如果其忙不过来才往后传递。

那么CPU收到中断信号后又如何处理呢？ARM共有7种工作模式，常规情况下会运行于用户模式(用户代码区)，一旦中断触发，会立刻切换至中断模式(响应函数)，中断模式分为IRQ(中断)和FIQ(快速中断)，它们二者的区别是，FIQ可以进一步中断IRQ。

由于是实战操作，过于理论的东西就不往上放了，如果要进一步了解ARM中断，可以参考这篇文章👉：https://my.oschina.net/u/914989/blog/121585

这里只需要掌握两个重要概念：
1. 中断号本身可以看作一种独立的CPU资源，通过中断控制器监听真实的物理资源(引脚)状态
2. CPU的外设中断会直接触发PC跳转到指定代码区

# Linux/IRQ基础

正如前文所说，中断是让CPU切换执行上下文，尽管Linux操作系统通过时间切片的方式实现多任务，但IRQ切换是硬件层级的，进入中断函数就意味着什么进程、调度、并发等软件概念将全部失效。举例来说，一个进程调用sleep只会让自身运行停止并让出CPU资源，但在内核中断函数当中sleep，那就真睡过去了——整个操作系统的调度机制都会崩溃掉。

所以，Linux将中断处理分为“`顶半部`”和“`底半部`”，可以简单粗暴地理解：
- 顶半部，硬件级响应，处理内容必须快准狠，尽快将CPU资源交还操作系统
- 底半部，交由系统任务队列调度，处理耗时的响应业务

打个比方，顶半部好比医院挂号，底半部好比排队就诊的过程。但不要死脑筋，如果响应业务本身并不耗时，就没必要再拆分为两个处理部分了，比如出院缴费，直接在顶半部搞定。

带着这个原则，看一下Linux中断编程接口：

```c
#include <linux/interrupt.h> 

// 根据GPIO引脚号获取对应中断号
int gpio_to_irq(unsigned gpio);

// 申请占用中断号，并绑定处理函数
//    - irq 中断号
//    - handler 顶半部中断处理函数
//    - flags 中断触发方式
//    - name  中断名称
//    - dev   中断参数传递
int request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags, const char *name, void *dev);

// 释放中断号
void *free_irq(unsigned int irq, void *dev);
```

下面来实际操作一把——Linux中断的顶半部处理实现。

# 最简单的GPIO中断

先来看个接线图，为了更好地展示中断，“继承”了上一篇文章的三色LED接线，预期要实现是“每按一次键改变一种颜色”。按键Key的两个脚接到了树莓派的`GPIO17`和`3V3`上，换句话说，就是用GPIO17接收上升沿中断信号。而LED的控制电路保持之前的不变。

![接线图](https://i.loli.net/2019/08/04/7EYIMusdiBWhv5Q.png)
![电路图](https://i.loli.net/2019/08/04/JlXWK7bcs4Igjxv.png)

实现GPIO上升沿中断大体分为4步：
1. 设置GPIO复用功能为输入模式 `gpio_request()`
2. 获取GPIO对应中断号 `gpio_to_irq()`
3. 申请中断号、中断类型、绑定处理函数 `request_irq()`
4. 释放中断(卸载驱动时) `free_irq()`

```c
#include <linux/module.h>
#include <linux/gpio.h>       // 各种gpio的数据结构及函数
#include <linux/interrupt.h>  // 内核中断相关接口

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Philon | https://ixx.life");

// 稍后由内核分配的按键中断号
static unsigned int key_irq = 0;

// 定义按键的GPIO引脚功能
static const struct gpio key = {
  .gpio = 17,         // 引脚号为BCM - 17
  .flags = GPIOF_IN,  // 功能复用为输入
  .label = "Key0"     // 标示为Key0
};

// 按键中断“顶半部”处理函数
static irqreturn_t on_key_press(int irq, void* dev)
{
  printk(KERN_INFO "key pressed\n");
  return IRQ_HANDLED;
}

static int __init gpiokey_init(void)
{
  int rc = 0;

  // 向内核申请GPIO
  if ((rc = gpio_request_one(key.gpio, key.flags, key.label)) < 0) {
      printk(KERN_ERR "ERROR%d: cannot request gpio\n", rc);
      return rc;
    }

  // 获取中断号
  key_irq = gpio_to_irq(key.gpio);
  if (key_irq < 0) {
    printk(KERN_ERR "ERROR%d:cannot get irq num\n", key_irq);
    return key_irq;
  }

  // 申请上升沿触发中断
  if (request_irq(key_irq, on_key_press, IRQF_TRIGGER_RISING, "onKeyPress", NULL) < 0) {
    printk(KERN_ERR "cannot request irq\n");
    return -EFAULT;
  }

  return 0;
}
module_init(gpiokey_init);

static void __exit gpiokey_exit(void)
{
  // 释放中断号及GPIO
  free_irq(key_irq, NULL);
  gpio_free(key.gpio);
}
module_exit(gpiokey_exit);
```

上述代码非常简单，就是在按下按键的时候，打印一条消息。可以通过`dmesg`命令查看内核打印消息：
```sh
philon@rpi:~/modules $ sudo insmod gpiokey.ko 
philon@rpi:~/modules $ dmesg
...
[   77.238326] gpiokey: no symbol version for module_layout
[   77.238345] gpiokey: loading out-of-tree module taints kernel.
[   79.310635] key pressed
[   79.463206] key pressed
[   79.463262] key pressed  # 我摸着右边的鼻孔对天发誓，我只按了一下！
```

正如文章最开始所说，Linux对各种资源的调用是有相关API的，要尽量使用内核接口编写驱动程序，一能保证底层代码的质量，二能提高代码的移植性。关于GPIO资源的调用，要熟悉以下接口：

```c
#include <linux/gpio.h>

struct gpio {
	unsigned	gpio;       // GPIO编号
	unsigned long	flags;  // GPIO复用功能配置
	const char	*label;   // GPIO标签名
};

// 单个GPIO资源申请/释放
int gpio_request_one(unsigned gpio, unsigned long flags, const char *label);
void gpio_free(unsigned gpio);

// 多个GPIO资源申请/释放
int gpio_request_array(const struct gpio *array, size_t num);
void gpio_free_array(const struct gpio *array, size_t num);

// GPIO状态读写
int gpio_get_value(unsigned gpio);
void gpio_set_value(unsigned gpio, int value);
```

# 按键防抖，中断的底半部与定时器接口

由于前边的代码没有做防抖，明明只按了一下按键，中断函数却被连续触发了3次。话说在单片机里实现按键防抖是非常简单的，无非就是睡个50毫秒，再确认是否真的按下即可。但是前文也明确说了，Linux是多任务系统，永远不要试图在中断函数里睡眠。因此，防抖只能放在Linux中断的底半部。

此外，慎用睡眠函数！除非你很清楚它不是忙等待。在多任务系统下，按键防抖的逻辑应该是——触发中断后，让出CPU资源50毫秒，然后再确认是否真的按下。

先来认识一下底半部机制，Linux内核提供的底半部机制主要有`软中断`、`tasklet`、`工作队列`、`线程IRQ`。

- 软中断，是有内核软件模拟的一种中断机制，注意不要和ARM指令触发的中断混淆，后者本质上是硬中断
- tasklet，基于软中断实现的中断调度机制，本质上还是中断，不允许在处理函数中sleep
- 工作队列，类似于tasklet，区别在于工作队列底层基于线程，可以在处理函数中sleep
- 线程IRQ，不用解释了，就是个线程

有关Linux底半部的知识不适合放在这里，建议参考此文：http://chinaunix.net/uid-20768928-id-5077401.html

这里了解底半部机制的目的，仅仅是为了挑选一种何时的响应方式，首先可以明确，软中断和tasklet不能睡，pass。线程维护麻烦，pass。就只剩工作队列了。尽管工作队列可以睡，但内核提供的`usleep/msleep`等接口本质上是忙等待，依旧占用CPU资源，pass。怎么办呢——工作队列+定时器。当中断来临后：
1. 顶半部迅速定义个工作队列，交由内核调度
2. 当工作队列被调度时，迅速定义个定时器——延时50ms
3. 当定时器到时中断，才真的去做防抖判断

**工作队列**API：
```c
#include <linux/workqueue.h>

// 工作队列原型
struct work_struct {
	atomic_long_t data;
	struct list_head entry;
	work_func_t func;
#ifdef CONFIG_LOCKDEP
	struct lockdep_map lockdep_map;
#endif
};

// 工作队列回调函数原型
typedef void (*work_func_t)(struct work_struct *work);

// 初始化一个工作队列，绑定回调
INIT_WORK(work, func);
// 启动队列，之后会由内核完成调度
schedule_work(&my_wq);
```

**定时器**API:
```c
#include <linux/timer.h>

// 全局变量
// 记录上电后定时器中断次数，也就是开机时长，但不是微秒或纳秒的概念
extern unsigned long volatile jiffies;
// 表示CPU一秒钟有多少个定时器中断
#define HZ 100

// 简单来说，如果要定义一个100ms的延时，相当于以下公式：
// jiffies + (HZ/10)
// 相当于以现在的jiffies做偏移，而1s的十分之一就是100ms

// 定时器原型
struct timer_list {
	struct hlist_node	entry;
	unsigned long		expires;
	void			(*function)(struct timer_list *);
	u32			flags;
#ifdef CONFIG_LOCKDEP
	struct lockdep_map	lockdep_map;
#endif
};

// 向内核注册一个定时器
#define timer_setup(timer, callback, flags)
void add_timer(struct timer_list *timer);

// 向内核删除一个定时器
int del_timer(struct timer_list *timer);

// 修改定时器的下次的jiffies
int mod_timer(struct timer_list *timer, unsigned long expires)
```

下面是本文的完整代码，按一次按键，切换一次彩色led的颜色：
```c
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h> // 混杂设备相关结构
#include <linux/gpio.h>       // 各种gpio的数据结构及函数
#include <linux/interrupt.h>  // 内核中断相关接口
#include <linux/workqueue.h>
#include <linux/timer.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Philon | https://ixx.life");

// 定义按键的GPIO引脚
static const struct gpio key = {
  .gpio = 17,         // 引脚号为BCM - 17
  .flags = GPIOF_IN,  // 功能复用为输入
  .label = "Key0"     // 标示为Key0
};

// 定义三色LED的GPIO引脚
static const struct gpio leds[] = {
  { 2, GPIOF_OUT_INIT_HIGH, "LED_RED" },
  { 3, GPIOF_OUT_INIT_HIGH, "LED_GREEN" },
  { 4, GPIOF_OUT_INIT_HIGH, "LED_BLUE" },
};

static unsigned int keyirq = 0;     // GPIO按键中断号
static struct work_struct keywork;  // 按键工作队列
static struct timer_list timer;     // 定时器作为中断延时

// 按键中断“顶半部”处理函数，启用工作队列
static irqreturn_t on_key_press(int irq, void* dev)
{
  schedule_work(&keywork);
  return IRQ_HANDLED;
}

// 按键中断“底半部”工作队列，启动一个50ms的延时定时器
void start_timer(struct work_struct *work)
{
  mod_timer(&timer, jiffies + (HZ/20));
}

// 按键防抖定时器，及处理函数
void on_delay_50ms(struct timer_list *timer)
{
  static int i = 0;
  if (gpio_get_value(key.gpio)) {
    gpio_set_value(leds[i].gpio, 0);
    i = ++i == 3 ? 0 : i;
    gpio_set_value(leds[i].gpio, 1);
  }
}

static int __init gpiokey_init(void)
{
  int rc = 0;

  // 向内核申请GPIO
  if ((rc = gpio_request_one(key.gpio, key.flags, key.label)) < 0
    || (rc = gpio_request_array(leds, 3)) < 0) {
      printk(KERN_ERR "ERROR%d: cannot request gpio\n", rc);
      return rc;
    }

  // 获取中断号
  keyirq = gpio_to_irq(key.gpio);
  if (keyirq < 0) {
    printk(KERN_ERR "can not get irq num.\n");
    return -EFAULT;
  }

  // 申请上升沿触发
  if (request_irq(keyirq, on_key_press, IRQF_TRIGGER_RISING, "onKeyPress", NULL) < 0) {
    printk(KERN_ERR "can not request irq\n");
    return -EFAULT;
  }

  // 初始化按键中断底半部(工作队列)
  INIT_WORK(&keywork, start_timer);

  // 初始化定时器
  timer_setup(&timer, on_delay_50ms, 0);
  add_timer(&timer);

  return 0;
}
module_init(gpiokey_init);

static void __exit gpiokey_exit(void)
{
  free_irq(keyirq, NULL);
  gpio_free_array(leds, 3);
  gpio_free(key.gpio);
  del_timer(&timer);
}
module_exit(gpiokey_exit);
```

# 小结

- ARM有7种工作模式，其中IRQ和FIQ为中断模式，会导致CPU跳转到指定代码区
- Linux/IRQ分为顶半部和底半部机制
- 顶半部处理要快且不是睡眠
- 底半部又分为4种机制，软中断、tasklet、工作队列、线程IRQ
- 我们可以通过gpio_xxx函数访问CPU资源，而无需地操作底层寄存器
- 如果有延时需求，最好采用内核提供的定时器接口