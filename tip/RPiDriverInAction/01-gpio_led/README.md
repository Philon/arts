# 树莓派驱动开发实战01：GPIO驱动之LED

本文源码：https://github.com/philon/rpi-drivers/tree/master/01-gpio-led

GPIO可以说是驱动中最最最简单的部分了，但我上网查了下，绝大部分所谓《树莓派GPIO驱动》的教程全是python、shell等编程，或者调用第三方库，根本不涉及任何ARM底层、Linux内核相关的知识。显然，这根本不是什么驱动实现，只是调用了一两个别人实现好的库函数而已，跟着那种文章走一遍，你只知道怎么用，永远不知道为什么。

所以本文是希望从零开始，在Linux内核下实现一个真正的gpio-led驱动程序，初步体验一下Linux内核模块的开发思想，知其然，知其所以然。

## GPIO基础

General-purpose input/output(通用输入/输出)，其引脚可由软件控制，选择输入、输出、中断、时钟、片选等不同的功能模式。以树莓派为例，我们可以通过[pinout官网](https://pinout.xyz)查看板子预留的40pinGPIO分别是做什么的。

![FD10F639-BBC1-4AB8-938C-7C69F3D005B4.png](https://i.loli.net/2019/07/20/5d32e43bd2d9783386.png)


如上图，GPIO0-1、GPIO2-3脚，除了常规的输入/输出，还可作为I²C接口，GPIO14-15脚，可另作为TTL串口。

总之，GPIO平时就是个普通IO口，仅作为开关用，但开关只是为了掩人耳目，背后的复用功能才是它的真正职业。

## 三色LED电路

弄懂了GPIO原理，那就来实际操作一把，准备点亮LED灯吧！

先来看看原理图，为了区分三色灯不同颜色的LED，我特别用红绿蓝接入对应的RGB三个灯，黑线表示GND。

![接线图](https://i.loli.net/2019/07/22/5d35d0e3877be80312.png)

如图所示，三色灯的R、G、B正极分别接到树莓派GPIO的2、3、4脚，灯的公共负极随便接一个GND脚。因此，想要点亮其中一个灯，对应GPIO脚输出高电平即可，是不是很简单呐！

![电路图](https://i.loli.net/2019/07/22/5d35d0e33a84a48133.png)

## BCM2837寄存器分配

基于上述，要点亮LED只需要做一件事——GPIO输出高电平。如果通过程序让GPIO口输出高电平呢？

GPIO的控制其实是通过对应的CPU寄存器来实现的。在ARM架构的SoC中，所有的外围资源(寄存器)其实都是被映射到内存当中的，所以我们要读写寄存器，只需访问它映射到的内存地址即可。

那么问题来了，为什么不直接读写CPU的寄存器呢？因为现代的嵌入式系统往往都标配内存模块，处理器也带有MMU，所以其内部寄存器也就交由MMU来管理。

综上，我们现在要找出树莓派3B+这款芯片——BCM2837B0的GPIO物理内存地址。

*这里不得不吐槽一下，我先是跑到[树莓派3B+官方网址](https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2837b0/README.md)去找芯片资料，得知BCM2837其实就是BCM2836的主频升级版；我又去看BCM2836的资料，得知这只不过是BCM2835从32位到64位的升级版；我又去看BCM2835的芯片资料，然而里面说的内存映射地址根本就是错的......*

要确定BCM2837B0的内存映射需要参考两个地方：
1. [BCM2835 Datasheet](https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2835/BCM2835-ARM-Peripherals.pdf)，但要留意，里面坑很多，且并不完全适用于树莓派3B。
2. [国外热心网友的代码](https://github.com/Filkolev/LED-basic-driver)，仅包含GPIO驱动，没有太多细节

官方文档在第6页和第90页有这样一句话和这样几张表：

> Physical addresses range from 0x20000000 to 0x20FFFFFF for peripherals. The bus addresses for peripherals are set up to map onto the peripheral bus address range starting at 0x7E000000. Thus a peripheral advertised here at bus address 0x7Ennnnnn is available at physical address 0x20nnnnnn.

![BCM2835 GPIO 总线地址分配](https://i.loli.net/2019/07/22/5d35d0e347d7391060.png)

![GPIO 复用功能选择](https://i.loli.net/2019/07/22/5d35d0e32653b98460.png)

我就直说吧，共6个关键要素：
- 外围总线地址0x7E000000映射到ARM物理内存地址0x20000000，加上偏移，**GPIO物理地址为0x20200000**
- GPIO操作需要先通过**GPFSEL选择复用功能**，再通过**GPSET/GPCLR对指定位拉高/拉低**
- BMC2835共54个GPIO，分为两组BANK，第一组[0:31]，第二组[32:53]
- GPFSEL寄存器每3位表示一个GPIO的复用功能，因此一个寄存器可容纳10个GPIO，共6个GPFSEL
- GPSET/GPCLR寄存器每1位表示一个GPIO的状态1/0，因此一个寄存器可容纳32个GPIO，共2个GPSET/GPCLR
- ⚠️国外热心网友指出：**树莓派3B+的GPIO物理内存地址被映射到了0x3F200000！**

好了，现在结合电路图可推导出思路：
1. R、G、B分别对应GPIO2、3、4，需要操作的寄存器为GPFSEL0/GPSET0/GPCLR0
2. 要把三个脚全部设为“输出模式”，需要将GPFSEL0的第6、9、12都置为001
3. 要控制三个脚的输出状态，需要将GPSET0/GPCLR0的第2、3、4脚置1

## 先点亮红灯

现在开始写Linux驱动模块，先不着急完整实现，这一步只是把其中的红灯点亮，为此我甚至把绿蓝线给拔了！

代码实现非常简单，就是在加载驱动时红灯亮，卸载驱动时红灯灭。
```c
#include <linux/init.h>
#include <linux/module.h>
#include <asm/io.h>

#define BCM2837_GPIO_BASE             0x3F200000
#define BCM2837_GPIO_FSEL0_OFFSET     0x0   // GPIO功能选择寄存器0
#define BCM2837_GPIO_SET0_OFFSET      0x1C  // GPIO置位寄存器0
#define BCM2837_GPIO_CLR0_OFFSET      0x28  // GPIO清零寄存器0

static void* gpio = 0;

static int __init rgbled_init(void)
{
  // 获取GPIO对应的Linux虚拟内存地址
  gpio = ioremap(BCM2837_GPIO_BASE, 0xB0);

  // 将GPIO bit2设置为“输出模式”
  int val = ioread32(gpio + BCM2837_GPIO_FSEL0_OFFSET);
  val &= ~(7 << 6);
  val |= 1 << 6;

  // GPIO bit2 输出1
  iowrite32(val, gpio);
  iowrite32(1 << 2, gpio + BCM2837_GPIO_SET0_OFFSET);

  return 0;
}
module_init(rgbled_init);

static void __exit rgbled_exit(void)
{
  // GPIO输出0
  iowrite32(1 << 2, gpio + BCM2837_GPIO_CLR0_OFFSET);
  iounmap(gpio);

}
module_exit(rgbled_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Philon | ixx.life");
```

这段代码太简单了，以至于我觉得完全不需要解释，直接看效果吧。图中的命令：

```sh
philon@rpi:~/modules$ insmod rgbled.ko  # 亮
philon@rpi:~/modules$ rmmod rgbled.ko   # 灭
```

![红灯点亮效果](https://i.loli.net/2019/07/20/5d3331f96425544265.gif)

## 再点亮全部

其实点亮红灯后，绿蓝灯无非是改改地址而已，没什么难度。本文的目的是学习Linux驱动，点亮LED不过是驱动开发的感性认识，所以我决定把简单的问题复杂化😄。驱动主要为用户层提供了几种设备控制方式：
1. 通过命令`echo [white|black|red|yellow...] > /dev/rgbled`直接控制灯的颜色
2. 通过命令`cat /dev/rgbled`查看当前灯的状态
3. 通过函数`ioctl(fd, 1, 0)`可独立控制每个灯的状态

说白了，用户层只须关心等的输出的颜色，屏蔽了具体的电路引脚及状态。

为此，我们需要把三色LED模块当作一个**字符设备**来实现，本文是驱动开发实战，所以更多的讲如何实现，有关字符设备的原理可以参考我的另一篇文章[《ARM-Linux驱动开发四：字符设备》](https://ixx.life/ArmLinuxDriver/chapter4/)。

驱动主要分为两大块：**设备的`read/write/ioctl`接口**以及**字符设备的注册**。

先看看驱动的读写控制是如何实现的：
```c
// 三色LED灯不同状态组合
static struct { const char* name; const bool pins[3]; } colors[] = {
  { "white",  {1,1,1} },  // 白(全开)
  { "black",  {0,0,0} },  // 黑(全关)
  { "red",    {1,0,0} },  // 红
  { "green",  {0,1,0} },  // 绿
  { "blue",   {0,0,1} },  // 蓝
  { "yellow", {1,1,0} },  // 黄
  { "cyan",   {0,1,1} },  // 青
  { "purple", {1,0,1} },  // 紫
};

static void* gpio = 0; // GPIO起始地址映射
static bool ledstate[3] = {0}; // 三个LED灯当前状态

void gpioctl(int pin, bool stat)
{
  void* reg = gpio + (stat ? BCM2837_GPIO_SET0_OFFSET : BCM2837_GPIO_CLR0_OFFSET);
  ledstate[pin-2] = stat;
  iowrite32(1 << pin, reg);
}

// 通过文件读取，得到当前颜色名称
ssize_t rgbled_read(struct file* filp, char __user* buf, size_t len, loff_t* off)
{
  int rc = 0;
  int i = 0;

  // 当文件已经读过一次，返回EOF，避免重复读
  if (*off > 0) {
    return 0;
  }
  
  // 根据当前三个LED的输出状态，找到对应颜色名，返回
  for (i = 0; i < sizeof(colors) / sizeof(colors[0]); i++) {
    const char* name = colors[i].name;
    const bool* pins = colors[i].pins;

    if (ledstate[0] == pins[0] && ledstate[1] == pins[1] && ledstate[2] == pins[2]) {
      char color[32] = {0};
      sprintf(color, "%s\n", name);
      *off = strlen(color);
      rc = copy_to_user(buf, color, *off);
      return rc < 0 ? rc : *off;
    }
  }

  return -EFAULT;
}

// 通过向文件写入颜色名称，控制LED灯状态
ssize_t rgbled_write(struct file* filp, const char __user* buf, size_t len, loff_t* off)
{
  char color[32] = {0};
  int rc = 0;
  int i = 0;

  rc = copy_from_user(color, buf, len);
  if (rc < 0) {
    return rc;
  }

  *off = 0; // 每次控制之后，文件索引都回到开始

  // 根据用户层传来的颜色名，找到对应引脚状态，输出
  for (i = 0; i < sizeof(colors) / sizeof(colors[0]); i++) {
    const char* name = colors[i].name;
    const bool* pins = colors[i].pins;
    if (!strncasecmp(color, name, strlen(name))) {
      gpioctl(LED_RED_PIN, pins[0]);
      gpioctl(LED_GREEN_PIN, pins[1]);
      gpioctl(LED_BLUE_PIN, pins[2]);
      return len;
    }
  }

  return -EINVAL;
}

// 通过ioctl函数控制每个灯的状态
long rgbled_ioctl(struct file* filp, unsigned int cmd, unsigned long arg)
{
  if (cmd >= 2 && cmd <= 4) {
    gpioctl(cmd, arg);
  } else {
    return -ENODEV;
  }

  return 0;
}

const struct file_operations fops = {
  .owner = THIS_MODULE,
  .read = rgbled_read,
  .write = rgbled_write,
  .unlocked_ioctl = rgbled_ioctl,
};
```

关于读/写/控制这三种操作的代码实现，看似复杂，其实都很容易理解，无非就是通过`copy_to_user`和`copy_from_user`两个函数，实现内核层与用户层之间的数据交互，剩下的事情不过就是在`colors`结构体数组中进行遍历和比对而已。

然后是字符设备注册、GPIO功能配置等内容的实现。每种字符设备都需要唯一的主设备号和次设备号，设备号可以静态指定或动态分配，原则上建议由内核动态分配，避免冲突。

字符设备的创建有很多种思路，普通字符设备、混杂设备、平台设备等，它们都是内核提供的编程框架。例如GPIO这类设备，内核其实是有专门的gpio类，但为了更好的学习驱动开发，别着急，一步步来，先从最简单的开始(因为难的我也不会)。

下边的代码主要看`cdev_xxx`相关的部分即可，驱动加载时配置好GPIO映射，注册字符设备，获取设备号；驱动卸载时，取消GPIO映射，释放设备号，注销字符设备。

```c
static dev_t devno = 0;   // 设备编号
static struct cdev cdev;  // 字符设备结构体

static int __init rgbled_init(void)
{
  // 映射GPIO物理内存到虚拟地址，并将其置为“输出模式”
  // 代码写得比较丑，解释以下：
  // 就是先把三个GPIO的“功能选择位”全部置000
  // 然后再将其置为001
  int val = ~((7 << (LED_RED_PIN*3)) | (7 << (LED_GREEN_PIN*3)) | (7 << LED_BLUE_PIN*3));
  gpio = ioremap(BCM2837_GPIO_BASE, 0xB0);
  val &= ioread32(gpio + BCM2837_GPIO_FSEL0_OFFSET);
  val |= (1 << (LED_RED_PIN*3)) | (1 << (LED_GREEN_PIN*3)) | (1 << (LED_BLUE_PIN*3));
  iowrite32(val, gpio);

  // 将该模块注册为一个字符设备，并动态分配设备号
  if (alloc_chrdev_region(&devno, 0, 1, "rgbled")) {
    printk(KERN_ERR"failed to register kernel module!\n");
    return -1;
  }
  cdev_init(&cdev, &fops);
  cdev_add(&cdev, devno, 1);

  printk(KERN_INFO"rgbled device major & minor is [%d:%d]\n", MAJOR(devno), MINOR(devno));

  return 0;
}
module_init(rgbled_init);

static void __exit rgbled_exit(void)
{
  // 取消gpio物理内存映射
  iounmap(gpio);

  // 释放字符设备
  cdev_del(&cdev);
  unregister_chrdev_region(devno, 1);

  printk(KERN_INFO"rgbled free\n");
}
module_exit(rgbled_exit);
```

代码基本就是这样。来看看效果吧，操作指令如下

```sh
# 编译驱动，拷贝至开发板
philon@a55v:~/drivers/01-gpio_led$ make
philon@a55v:~/drivers/01-gpio_led$ scp rgbled.ko rgbled_test rpi.local:/home/philon/modules

#----------------------以下是开发板操作----------------------
# 加载驱动 & 查看模块的主从设备号
philon@rpi:~/modules$ sudo insmod rgbled.ko
philon@rpi:~/modules$ dmesg 
...
[  106.818009] rgbled: no symbol version for module_layout
[  106.818028] rgbled: loading out-of-tree module taints kernel.
[  106.820307] rgbled device major&minor is [240:0] 👈主从设备号

# 根据动态分配的设备号创建设备节点
philon@rpi:~/modules$ sudo mknod /dev/rgbled c 240 0

philon@rpi:~/modules$ sudo sh -c "echo green > /dev/rgbled" # 打开绿灯
philon@rpi:~/modules$ sudo ./rgbled_test b 1                # 再打开蓝灯
philon@rpi:~/modules$ sudo cat /dev/rgbled                  # 查看当前颜色
cyan #青色
```

PS：动态图，只是被我设置得比较慢，别着急换台呀！😂

![三色LED驱动最终效果图](https://i.loli.net/2019/07/22/5d35cca94f89546830.gif)