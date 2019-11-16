# 树莓派驱动开发实战07：PDD与设备树

本文源码：https://github.com/Philon/rpi-drivers/tree/master/07-pdd

从《树莓派驱动开发实战》的第一篇至今，都是在写单个字符设备，这其中不难发现个问题——如果我有10个LED灯就意味着我要写10个led字符设备驱动，而其中的大部分代码都是重复的，它们之间可能仅仅是控制引脚不同。

一是为了解决这个问题，二是之后的驱动开发更多会涉及USB、I²C、UART之类的总线设备，三是为了更好地理解Linux驱动架构。从本篇文章开始正式以`驱动-总线-设备`模型和`设备树`机制来编写设备驱动。我觉得以**平台驱动设备**模型作为切入点较好——可以不涉及真实的硬件。

## 驱动-总线-设备模型

Linux2.6之后引入了全新驱动注册管理机制：**驱动、总线、设备**。一句话，为了高内聚低耦合！

- 驱动部分：负责实现设备的控制逻辑及用户接口，并注册到内核
- 设备部分：负责描述设备的硬件资源，并告知内核
- 总线部分：负责实现设备与驱动之间的感知、识别、匹配规则

举例来说，如果有100个按键(比如键盘)，我只需要实现`1个按键驱动`+`100个按键设备描述`，并把它们挂到`按键总线`上，总线会负责把二者匹配起来，所有的按键就都可以用了。

内核提供了相应的`bus、device、driver、class`等最为底层的API和数据结构，即`驱动、总线、设备`模型来管理系统设备。但在日常驱动开发中，一般是用不到的。因为常见物理总线都基于这些API封装了对应的如`usb_bus、usb_device、usb_driver、tty_device、tty_driver`等接口，日常的设备驱动开发更多以这一层打交道。

用面向对象的思想来说，驱动总线设备模型就是DeviceManage基类，由此派生出了USB、TTY、I²C、SPI、PCIe、GPIO等设备管理机制。当然，这其中也包括`platform`平台设备管理。

```sh
philon@rpi:~ $ ls /sys/bus/
amba         container     genpd  i2c              media     mmc_rpmb  scsi  usb
clockevents  cpu           gpio   iscsi_flashnode  mipi-dsi  nvmem     sdio  workqueue
clocksource  event_source  hid    mdio_bus         mmc       platform  spi
```

通过`/sys/bus`目录可以看到系统当前存在的总线。👆注意看倒数第二个，`platform`平台总线出现了！！

## 平台总线、平台驱动、平台设备

**platform是一种虚拟总线**。它是“驱动-总线-设备”模型的一种实现，与`usb_bus tty_bus spi_bus`等物理总线平级。为了把那些不走总线架构的设备囊括进来。

回顾历史：

1. 编写字符设备`cdev`时需要关心主、次设备号，还要用mknod命令创建对应的设备节点。
2. 于是内核提供`misc`混杂设备，将所有不好管理的字符设备统一主设备号为10，自动分配和创建节点，本质上就是基于cdev再封装了一层。
3. 为了管理总线设备，内核又提出了`驱动总线设备`模型，并封装了各种USB、I²C、TTY等软件层。
4. 为了把非总线架构的设备也用总线思想来管理，内核提出了`平台驱动设备`层

所以，`platform`和`misc`一样，都是为了给`“其他”`设备找一个爸爸。

## 最简单的PDD实例

根据上述可推断，`platform_bus`(即总线部分)内核已经实现了，所以我们只需要实现两边的`platform_xxx_driver`和`platform_xxx_device`即可，然后把它们挂到平台总线上去，总线会自动进行匹配的。以下只是以led为例说明platform相关接口用法，并未真正实现led驱动。

led_driver.c

```c
#include <linux/module.h>
#include <linux/platform_device.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Philon | https://ixx.life");

// 当总线匹配到设备时调用该函数
static int led_probe(struct platform_device* dev) {
  printk("led %s probe\n", dev->name);
  // todo: 字符设备注册、gpio申请之类的
  return 0;
}

// 当总线匹配到设备卸载时调用该函数
static int led_remove(struct platform_device* dev) {
  printk("led %s removed\n", dev->name);
  // todo: 其他资源释放
  return 0;
}

// 平台驱动描述
static struct platform_driver led_driver = {
  .probe = led_probe,
  .remove = led_remove,
  .driver = {
    .name = "my_led", // 👈务必注意，platform是以name比对来匹配的
    .owner = THIS_MODULE,
  },
};

// 【宏】将led驱动挂到平台总线上
// 相当于同时定义了模块的入口和出口函数
// module_init(platform_driver_register)
// module_exit(platform_driver_unregister)
module_platform_driver(led_driver);
```

led_device.c

```c
#include <linux/module.h>
#include <linux/platform_device.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Philon | https://ixx.life");

// ⚠️最好实现该接口，否则在设备释放的时候内核会报错
static void led_release(struct device* pdev) {
  printk("led release!\n");
}

// 平台设备描述
static struct platform_device led_device = {
  .name = "my_led", // 👈要确保与led_driver的定义一致，否则匹配不上
  .dev = {
    .release = led_release,
  },
};

// 将设备注册到平台总线
static int leddev_init(void) {
  platform_device_register(&led_device);
  return 0;
}
module_init(leddev_init);

static void leddev_exit(void) {
  platform_device_unregister(&led_device);
}
module_exit(leddev_exit);
```

简述一下代码的逻辑：

1. platform_bus监听到有device注册时，会查看它的`device.name`
2. platform_bus会查找所有的`driver.name`，找到之后将设备和驱动进行绑定
3. 绑定成功后，`platform_driver.probe()`将触发，刚才的设备作为参数传递进去
4. 剩下的事情，就看你如何实现platform_driver了...

实际操作下，加载led_driver.ko模块后，可以在平台总线目录下看到`my_led`驱动了。然后，加载led_device.ko模块后，同样可以在平台总线设备里查看到`my_led.0`的设备。

```sh
# 平台总线里查看my_led驱动
philon@rpi:~/modules $ sudo insmod led_driver.ko 
philon@rpi:~/modules $ ls /sys/bus/platform/drivers/my_led/
bind  module  uevent  unbind

# 平台总线里查看my_led设备
philon@rpi:~/modules $ sudo insmod led_device.ko      
philon@rpi:~/modules $ ls /sys/bus/platform/devices/my_led.0/
driver  driver_override  modalias  power  subsystem  uevent

# 看一下内核打印信息
philon@rpi:~/modules $ dmesg
[225668.547712] led my_led probe
[225687.213336] led my_led removed
[225687.213448] led release!
```

⚠️温馨提示：不必操心driver/device模块的加载顺序，谁先谁后都一样，platform_bus会料理好一切。

以上，便是PDD模型的一个基本展示，如果你愿意，可以在led_device.c文件里多注册几个设备。不过在此之前——你内心难道不会充满疑惑吗：这tm怎么匹配上的呀？🤔️

## 平台驱动和设备的匹配

看一下内核是如何实现平台匹配的，非常容易理解：

```c
// linux-rpi-4.19.y/drivers/base/platform.c line:963
static int platform_match(struct device *dev, struct device_driver *drv)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct platform_driver *pdrv = to_platform_driver(drv);

	/* When driver_override is set, only bind to the matching driver */
	if (pdev->driver_override)
		return !strcmp(pdev->driver_override, drv->name);

	/* 首先尝试设备树匹配(OF - Open Firmware Standard) */
	if (of_driver_match_device(dev, drv))
		return 1;

	/* 然后尝试匹配高级配置和电源接口
     (ACPI - https://baike.baidu.com/item/ACPI/299421?fr=aladdin)
   */
	if (acpi_driver_match_device(dev, drv))
		return 1;

	/* 然后尝试匹配ID表 */
	if (pdrv->id_table)
		return platform_match_id(pdrv->id_table, pdev) != NULL;

	/* 最后尝试匹配驱动和设备的名称 */
	return (strcmp(pdev->name, drv->name) == 0);
}
```

从以上代码可以看出，平台总线的匹配经过`设备树 > ACPI > ID表 > 名称`等4种方式匹配，只要任意一种属性确认过眼神，就可以进行下一步。所以`led_driver`和`led_device`能够匹配上，正是因为它们内部的`name`值相同。

接着，看看平台驱动和平台设备的数据结构，一切就明朗了。

在平台驱动的数据结构中可以看到，它内部包含了底层的`device_driver`结构，如果驱动想要只是某些类型的设备，那就必须在相应的用于匹配的属性里事先声明。

```c
struct platform_driver {
	int (*probe)(struct platform_device *);
	int (*remove)(struct platform_device *);
	void (*shutdown)(struct platform_device *);
	int (*suspend)(struct platform_device *, pm_message_t state);
	int (*resume)(struct platform_device *);
	struct device_driver driver;  // 底层驱动数据结构
	const struct platform_device_id *id_table; // ID的匹配表👈
	bool prevent_deferred_probe;
};

struct device_driver {
	const char		    *name;      // 驱动名，用于名称匹配👈
	struct bus_type		*bus;       // 总线类型，如platform_bus
	struct module		*owner;     // 这就不解释了
	const char		    *mod_name;  // 用于构建模块
	bool suppress_bind_attrs;	/* disables bind/unbind via sysfs */
	enum probe_type probe_type;

	const struct of_device_id	*of_match_table;  // 设备树的匹配表👈
	const struct acpi_device_id	*acpi_match_table; // acpi的匹配表👈

	int (*probe) (struct device *dev);    // 探测设备：当匹配成功时回调
	int (*remove) (struct device *dev);   // 移除设备：当设备卸载时回调
	void (*shutdown) (struct device *dev); // 关闭设备
	int (*suspend) (struct device *dev, pm_message_t state); // 暂停
	int (*resume) (struct device *dev); // 恢复

	const struct attribute_group **groups;
	const struct dev_pm_ops *pm;  // 电源管理
	void (*coredump) (struct device *dev); // 核心转储
	struct driver_private *p; // 私有数据
};
```

和平台驱动很类似，其内部同样包含了底层的`device`结构体，如果设备想要被总线匹配上，同样要在自己的属性里配置好。

```c
struct platform_device {
	const char	  *name;    // 设备名，与驱动的name匹配 👈
	int		        id;       // 设备ID
	bool		      id_auto;
	struct device	dev;      // 底层数据结构
	u32		num_resources;    // 设备要用的资源数量
	struct resource	*resource;  // 设备的硬件资源描述

	const struct platform_device_id	*id_entry;  // 设备ID，与驱动的ID表匹配 👈
	char *driver_override; /* Driver name to force a match */

	/* MFD cell pointer */
	struct mfd_cell *mfd_cell;

	/* arch specific additions */
	struct pdev_archdata	archdata;
};
```

好了，本文并不打算详细讨论有关平台、驱动、设备及其如何匹配的原理。如果对此感兴趣或想要深入研究，请用好互联网。我个人也查了很多资料，感觉这位作者写的[《Linux Platform驱动模型(一) _设备信息》](https://www.cnblogs.com/xiaojiang1025/p/6367061.html)和[《Linux Platform驱动模型(二) _驱动方法》](https://www.cnblogs.com/xiaojiang1025/archive/2017/02/06/6367910.html)还阔以，适合入门。

那么接下来，我们已经知道平台总线提供了4种匹配方法，`name`匹配就不说了，另外两种不提也罢，最最最重要的`设备树`匹配该登场了。

## 设备树

先声明，关于设备树的语法、树莓派的配置规则，不会涉及太多。本文侧重于实战，原理知识请用好互联网。

从历史上说，ARM-Linux引入设备树完全是被逼出来的。我们知道ARM以IP授权的商业模式运作，诞生了众多芯片厂商。它不像x86/x64架构只有Intel之类的寡头，产品大同小异比较好管理。arm的江湖可谓鱼龙混杂，每家都想在Linux内核种争的一席之地。可偏偏这帮家伙把又是硬件出生，对于兼容自家的不同产品只会用`if-else`，作为软件大神的Linus自然是怒了：“策略模式”难道不香么，你以为用C写一堆“电路板说明书”很高级么？如果你愿意，可以浏览下内核目录`arch/arm/mach-xxx`，非常多对吧。其实这些目录大多是SoC的硬件细节描述，用于适配各大厂商不同型号的处理器或开发板。

闲话就扯那么多，总之，设备树就是用类C的文本语言编写，用于描述Soc及其外围电路模块的配置文件。通常情况下它由bootloader传递给内核。这种做法，极大的降低了驱动的维护难度，也大大增加了系统设备管理的灵活性。

⚠️在阅读下文前，必须基本懂得两个知识点：

1. 设备树语法，我就不多嘴了，网上一搜一大堆
2. 树莓派overlay机制，这个网上几乎没有，我做个大概说明

### 树莓派的设备树配置

本小结是从👉[树莓派DeviceTree的官方介绍中](https://www.raspberrypi.org/documentation/configuration/device-tree.md)总结而来，如果想更全面地了解，可以看原文。

一个常规的Arm-Linux设备树，主要是由源文件`.dts`和头文件`.dtsi`共同编译出`.dtb`二进制，内核在初始化后会加载这个dtb，并把相关设备都注册好，就可以愉快地使用了。例如树莓派3B+，`/boot/bcm2710-rpi-3-b-plus.dtb`就是树莓派SoC和外围电路的默认配置。

对于大部分硬件产品来说这没什么问题，例如一部手机在出厂以后，它的硬件几乎是不会变的。但对于树莓派这种开发板来说，尤其是它的40pin扩展引脚，外围电路的变动可就大了去了，而内核加载`dtb`后是不能变的，所以需要一种**动态覆盖配置**的设备树机制，这就是树莓派的——dtoverlay(设备树覆盖)。

dtoverlay同样是由dts源编译而来，语法几乎和设备树一样，不过输出文件扩展名为`dtbo`。树莓派提供了两种方式加载dtbo：

1. 将编译好的dtbo放到`/boot/overlays`下，并由`/boot/config.txt`配置和使能；
2. 通过命令`dtoverlay <dtbo_file>`动态覆盖设备树；

第1种方式会涉及更复杂的语法规则，本篇文章仅仅是对平台设备及设备树的知识入门，因此选择第2种命令行的方式，动态加载。

### 用设备树注册设备

led_driver.c：  
其他内容不变，仅仅是增加`of_device_id`属性。

```c
// 首先用of_device_id声明了三种LED型号的表，支持设备树解析
static const struct of_device_id of_leds_id[] = {
  { .compatible = "led_type_a" },
  { .compatible = "led_type_b" },
  { .compatible = "led_type_c" },
};

static struct platform_driver led_driver = {
  .probe = led_probe,
  .remove = led_remove,
  .driver = {
    .name = "my_led",
    .of_match_table = of_leds_id, // 👈在驱动种添加对应属性
    .owner = THIS_MODULE,
  },
};
```

接着新建一个设备树文件，并定义一个`led_type_a`的LED设备，并将其命名为`led_a1`。

myled.dts

```
/dts-v1/;
/plugin/;

/ {
  fragment@0 {
    target-path = "/";
    __overlay__ {
      led_a1 {
        compatible = "led_type_a";
      };
    };
  };
};
```

`fragment`和`__overlay__`非常重要！！如果不这么写会导致动态加载失败，但其实以上的代码转化为标准的设备树语法为：

```
/led_a1 {
  compatible = "led_type_a";
};
```

最后用`dtc`编译器将`dts`编译为`dtbo`：

```sh
linux-rpi-4.19.y/scripts/dtc -I dts -o myled.dtbo myled.dts
```

万事俱备，看看效果吧：

```sh
# 第一步：加载led驱动
philon@rpi:~/modules $ sudo insmod led_driver.ko 
# 第二步：加载设备树覆盖
philon@rpi:~/modules $ sudo dtoverlay myled.dtbo
# 第三部：看看平台设备里是否注册了一个叫“led_a1”的设备
philon@rpi:~/modules $ ls /sys/devices/platform/
alarmtimer  Fixed MDIO bus.0    👉led_a1👈  power         serial8250  uevent
...

# 显然已经注册，根据led_driver的实现，设备注册后会在probe函数中打印一条消息
philon@rpi:~/modules $ dmesg
...
[  429.359567] leddrv: no symbol version for module_layout
[  429.359577] leddrv: loading out-of-tree module taints kernel.
[  435.995744] led led_a1 probe 👈啊～我看到树上长了个灯
```

再来回顾下流程：

1. 首先驱动要支持`of_device_id`属性，并且以`compatible`作为匹配对象
2. 然后通过编写设备树定义相应的设备资源
3. 最后通过加载驱动和dtoverlay即可

### 让设备开机自动注册

这就非常简单了，前面已经说过`/boot/overlays`其实是通过`config.txt`配置和使能的，所以我们只需要将`myled.dtbo`放到overlays目录下，并在config.txt添加一行使能即可。

```sh
# 第一步：将自己的dtbo放到overlays下
philon@rpi:~/modules $ sudo cp myled.dtbo /boot/overlays
# 第二步：在config.txt最后一行添加myled
philon@rpi:~/modules $ sudo echo "dtoverlay=myled" | sudo tee -a /boot/config.txt
# 第三步：reboot...
# 第四步：可以在/sys/device/platform下查看到设备已经注册
philon@rpi:~/modules $ ls /sys/devices/platform/
alarmtimer  Fixed MDIO bus.0    👉led_a1👈  power         serial8250  uevent
...

# ⚠️但是，设备树仅仅是定义了led_device，而led_driver.ko其实并没有开机加载，如果要更完善的话，应该把led_driver直接编译进内核！
philon@rpi:~/modules $ sudo insmod leddrv.ko 
philon@rpi:~/modules $ dmesg
...
[  214.076752] leddrv: no symbol version for module_layout
[  214.076771] leddrv: loading out-of-tree module taints kernel.
[  214.077535] led led_a1 probe
```

## 小结

- Linux-2.6后引入了`驱动-总线-设备`的软件架构来管理系统设备；
- `platform`设备和USB、TTY、UART一样，都是基于底层的抽象和封装；
- `platform`是为了把那些没有总线的设备，以总线的思想管理起来，所以它算作一根虚拟总线；
- 平台总线提供了多种驱动和设备的匹配规则：设备树、ACPI、ID表、名称等；
- 设备树是由bootloader传递给内核，并且在初始化后基本不可修改；
- 树莓派为了满足设备树动态修改的需求，引入了`dtoverlay`；
- dtoverlay采用常规的设备树语法，但需要`fragment`和`__overlay__`属性；
- 驱动必须定义`of_device_id`数据结构，才能与设备树匹配；
- 务必掌握设备树语法！