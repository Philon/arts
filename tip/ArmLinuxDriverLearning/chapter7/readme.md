# 嵌入式Linux驱动学习七：中断与时钟

## ARM的中断类型

- SGI(Software Generated Interrupt): 软件中断，由程序自行产生，常用于多核通信
- PPI(Private Peripheral Interrupt): 私有中断，中断信号只绑定给某个CPU
- SPI(Shared Peripheral Interrupt): 共享中断，中断信号可以路由给任何CPU

## 顶半部和底半部

- 顶半部(Top Half)：处理紧急的硬件操作
- 底半部(Bottom Half)：处理耗时的业务逻辑

## GPIO按键中断

中断API：
```c
request_irq()
free_irq()

enable_irq()
disable_irq()
disable_irq_nosync()
```

## 中断共享

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