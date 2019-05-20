# 嵌入式Linux驱动学习七：中断与时钟

本篇学习笔记除了两本书籍外，同时参考博文：https://blog.csdn.net/u010503121/article/details/80502175

## ARM的中断类型

- SGI(Software Generated Interrupt): 软件中断，由程序自行产生，常用于多核通信(一个CPU通过写寄存器中断另一个CPU)
- PPI(Private Peripheral Interrupt): 私有中断，中断信号只绑定给某个CPU
- SPI(Shared Peripheral Interrupt): 共享中断，中断信号可以路由给任何CPU

## 顶半部和底半部

- 顶半部(Top Half)：处理优先级较高、紧急的硬件操作
- 底半部(Bottom Half)：处理优先级较低、耗时的业务逻辑

中断的处理之所以被拆分为顶半部和底半部，其实就是为了实时性，本质上两个“半部”都是为了处理同一个中断信号的相关业务逻辑，只不过这个业务逻辑可能会有轻重缓急之分，那么就把轻和急的交给顶半部，把重且缓的交给底半部。

举个例子：医院的本质是给人看病，每个到访医院的病人就相当于是一次中断信号，其背后是一次完整的就诊过程。但要处理的病人多如牛毛，所以医院会先给每个病人快速挂号——顶半部，然后再去做耗时的诊断和医治——底半部。但有些病人可能不是来看病的，比如办理出院、交钱，这种事情就没必要拆分成两个流程来处理了，直接当场解决。

主要记住一点：**顶半部不可被中断，底半部可以**。实际的中断实现要具体问题具体分析，常规情况下我们只在顶半部完成硬件信号的登记和关键信息记录，剩下的交给底半部做具体响应；但如果某些中断业务于硬件贴合非常紧密而且对实时性要求过高，那最好全部放在顶半部处理。

## 顶半部中断编程

顶半部中断的实现相对简单：

```c
/**
  中断注册
  @irq      申请的硬件中断号
  @handler  中断事件响应函数，dev_id参数将被传递给它
  @flags    中断处理标志，上升沿触发，下降沿触发
  @devname  设置中断名称，通常是设备驱动程序的名称
  @dev      中断处理函数的传入参数
 */
int request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags, const char *name, void *dev);

/**
  中断释放
  @irq  中断号
  @dev  由request时传入的dev参数
 */
void free_irq(unsigned int irq,void *dev);

/**
  @irq  中断号
  @dev  由request时传入的dev参数
 */
static irqreturn_t intrrupt_handler(int irq, void* dev);
```

## 底半部中断编程

一般来说，顶半部执行完底半部实现

### 1. tasklet

### 2. 工作队列

### 3. 软中断

### 4. threaded_irq

## 内核定时器

```c
struct timer_list
init_timer()
add_timer()
del_timer()
mod_timer()
```

## 内核延时

```c
ndelay()
udelay()
mdelay()
```