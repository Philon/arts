# 00 HelloWorld

本文源码：https://github.com/philon/rpi-drivers/tree/master/00-hello

最近打算利用手里的树莓派3B+及各种丰富的扩展模块学些一下嵌入式Linux驱动开发。我计划实现LED、温湿度传感器、陀螺仪、PS遥感、红外模组、超声波模组、光敏传感器等驱动，其目的是为了学习嵌入式Linux驱动开发的思想，以及常见的接口及模块的原理。我会把整个学习过程整理为《树莓派驱动开发实战》。当然，我的初衷是学习ARM-Linux驱动开发，如果不是必须，我不会可以强调硬件平台，而是更侧重于驱动编程和电路设计。

![](https://i.loli.net/2019/07/12/5d28aa437927829767.jpg)

我的开发环境如下：
1. 宿主机：华硕A55VM | Ubuntu18.04
2. 开发板：Raspberry 3 Model B+ 2017
3. 编辑器：Visual Studio Code | C/C++扩展

由于文章中会存在大量命令脚本的引用，先做个声明：
```sh
philon@a55v:~$ # 表示在宿主机敲出的命令

philon@rpi:~$  # 表示在开发板敲出的命令
```

此外，如果不是必须，我不会拿串口调试开发板，一律ssh！

# 准备工作

Linux驱动开发需要准备几样东西：
- `Datasheet`：硬件手册，用于了解目标平台的规格/寄存器/内存地址/引脚定义等
- `编译器`：用于编译目标平台的驱动源码，嵌入式开发，一般用交叉编译器
- `内核源码`：编译驱动依赖于内核，**且必须与目标平台系统内核版本一致**
- `外围电路原理图`：连怎么走线都不知道，还开发个球啊！

本文不涉及真实的模块驱动开发，因此具体的外围电路等实际开发的时候再看吧，先把前三样搞到手。

## 树莓派3B+硬件资料

由于官网没有配套的树莓派3B+Datasheet，我只能尽可能从官网其他地方把硬件资料凑齐。任何有关硬件的资料都优先从官网找：https://www.raspberrypi.org/documentation/hardware/raspberrypi/

树莓派3B+硬件介绍  

- 处理器是BCM2837B0，64bit四核1.4GHz的Cortex-A53架构
- 双频80211ac无线和蓝牙4.2
- 千兆有线网卡，且支持PoE(网线供电)

电路原理图下载：https://www.raspberrypi.org/documentation/hardware/raspberrypi/schematics/rpi_SCH_3bplus_1p0_reduced.pdf

## 宿主机安装交叉编译器

交叉编译器选择是有套路的：`<arch>-<vendor>-<target>`

例如：
- arm-linux-gnueabi-gcc 表示arm32位架构，目标可执行文件依赖Linux+glibc
- arm-linux-gnueabihf-gcc 表示arm32位/硬浮点数处理架构，其他同上
- aarch64-linux-gnueabi-gcc 表示arm64位架构，其他同上


所以在选择交叉编译器之前要先确定开发板的处理器及操作系统环境：
```sh
philon@rpi:~ $ uname -a
Linux rpi 4.19.42-v7+ #1219 SMP Tue May 14 21:20:58 BST 2019 armv7l GNU/Linux
```

注意最后的**armv7**，尽管RPi-3B+的处理器平台是64位4核，不过它的操作系统是32位的，所以应该安装arm32位glibc的交叉编译器。

```sh
philon@a55v:~$ sudo apt install gcc-arm-linux-gnueabihf
```

## 宿主机构建内核源码

Linux驱动模块的编译依赖于内核源码，而且要注意版本问题。之前在开发板上查看Linux版本时已经知道其运行的内核是`Linux rpi 4.19.42-v7+`。所以最好是去官网下载对应版本号的内核源码。

```sh
# 下载内核源码
philon@a55v:~$ wget https://github.com/raspberrypi/linux/archive/rpi-4.19.y.tar.gz
# 解压，进入内核目录
philon@a55v:~$ tar xvf rpi-4.19.y.tar.gz && cd linux-rpi-4.19.y
# 清理内核
philon@a55v:~/linux-rpi-4.19.y$ make mrproper
# 加载RPi-3B+的配置
philon@a55v:~/linux-rpi-4.19.y$ make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcm2709_defconfig
# 预编译
philon@a55v:~/linux-rpi-4.19.y$ make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules_prepare -j8
```

【题外话】：很多教程都是先把内核完整编译一遍，再拷贝各种头/库文件到目标平台，其实没必要，预编译内核即可。从[内核官方文档](https://www.kernel.org/doc/Documentation/kbuild/modules.txt)可以看到这样一段话：

> === 2. How to Build External Modules  
> To build external modules, you must have a prebuilt kernel available
that contains the configuration and header files used in the build.  
> ...  
> An alternative is to use the "make" target "modules_prepare." This will
make sure the kernel contains the information required. The target
exists solely as a simple way to prepare a kernel source tree for
building external modules.  
> NOTE: "modules_prepare" will not build Module.symvers even if
CONFIG_MODVERSIONS is set; therefore, a full kernel build needs to be
executed to make module versioning work.

简而言之，如果要构建外部驱动模块，内核必须有相关的配置及头文件。可以用“modules_prepare”准备内核源码树，这也仅仅用于构建外部驱动模块，但不会生成“Module.symvers”。

<!-- ## 安利我的开发环境

1. **代码编写**：VSCode和C/C++扩展，可以满足绝大多数的开发场景

需要配置头文件所在路径，可以从内核源码的Makefile中看到其对头文件的路径：
```Makefile
philon@a55v:~/drivers$ cat linux-rpi-4.19.y/Makefile

...
402 # Use USERINCLUDE when you must reference the UAPI directories only.
403 USERINCLUDE    := \
404                 -I$(srctree)/arch/$(SRCARCH)/include/uapi \
405                 -I$(objtree)/arch/$(SRCARCH)/include/generated/uapi \
406                 -I$(srctree)/include/uapi \
407                 -I$(objtree)/include/generated/uapi \
408                 -include $(srctree)/include/linux/kconfig.h
409 
410 # Use LINUXINCLUDE when you must reference the include/ directory.
411 # Needed to be compatible with the O= option
412 LINUXINCLUDE    := \
413                 -I$(srctree)/arch/$(SRCARCH)/include \
414                 -I$(objtree)/arch/$(SRCARCH)/include/generated \
415                 $(if $(KBUILD_SRC), -I$(srctree)/include) \
416                 -I$(objtree)/include \
417                 $(USERINCLUDE)
```

可以看到内核对头文件的依赖：`$(srctree)/include`和`$(srctree)/arch/arm/include`，以及它们的子目录。  
所以vscode的`c_cpp_properties.json`可以这么配置：
```json
"includePath": [
    "/home/philon/linux-rpi-4.19.y/include",
    "/home/philon/linux-rpi-4.19.y/include/uapi",
    "/home/philon/linux-rpi-4.19.y/include/generated",
    "/home/philon/linux-rpi-4.19.y/include/generated/uapi",
    "/home/philon/linux-rpi-4.19.y/arch/arm/include",
    "/home/philon/linux-rpi-4.19.y/arch/arm/include/uapi",
    "/home/philon/linux-rpi-4.19.y/arch/arm/include/generated",
    "/home/philon/linux-rpi-4.19.y/arch/arm/include/generated/uapi"
],
```

如此一来就可以非常愉快地写内核代码了。
![](https://i.loli.net/2019/03/25/5c98ef6a9633b.gif)

2. **远程链接**，SSH密钥登陆

建议在目标板启用sshd并采取密钥登陆的方式，有关“SSH密钥登陆”的方式自行搜索，非常简单。

此外，如果局域网比较完善的话，建议启用本地域名，就不用每次都操心IP的事情啦，比如：
```sh
# 宿主机ping目标板
philon@a55v:~/$ ping rpi.local
PING rpi.local (192.168.1.28) 56(84) bytes of data.
64 bytes from 192.168.1.28 (192.168.1.28): icmp_seq=1 ttl=64 time=232 ms
64 bytes from 192.168.1.28 (192.168.1.28): icmp_seq=2 ttl=64 time=2.44 ms

# 宿主机登陆目标板
philon@a55v:~$ ssh philon@rpi.local
```

3. **文件共享**，NFS或scp

宿主机生成的驱动模块文件需要远程拷贝到目标板，方法千千万，这里只推荐两种：

- 宿主机搭建nfs服务，目标板通过挂载的方式共享目录
```sh
philon@rpi:~ $ mount -o nolock a55v.local:/nfs_path /mnt
```

- 直接使用`scp`命令远程拷贝到目标板(如果懒得搭建NFS的话)
```sh
philon@a55v:~/drivers/00-hello$ scp hello.ko rpi.local:/home/philon/modules
``` -->


# 第一个树莓派驱动

入门例子当然是留给HelloWorld啦！

创建工程目录：
```sh
# 创建目录，进入
philon@a55v:~$ mkdir -p drivers/00-hello && cd drivers/00-hello
# 创建驱动模块源码及Makefile
philon@a55v:~/drivers/00-hello$ touch hello.c Makefile
```

hello.c
```c
#include <linux/init.h>
#include <linux/module.h>

static int __init hello_init(void)
{
  printk("hello kernel\n");
  return 0;
}
module_init(hello_init);

static void __exit hello_exit(void)
{
  printk("bye kernel\n");
}
module_exit(hello_exit);

MODULE_LICENSE("GPL v2"); // 开源许可证
MODULE_DESCRIPTION("hello module for RPi 3B+"); // 模块描述
MODULE_ALIAS("Hello"); // 模块别名
MODULE_AUTHOR("Philon"); // 模块作者
```

Makefile
```Makefile
# 模块驱动，必须以obj-m=xxx形式编写
obj-m = hello.o

KDIR = ../linux-rpi-4.19.y
CROSS = ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-

all:
	$(MAKE) -C $(KDIR) M=$(PWD) $(CROSS) modules

clean:
	$(MAKE) -C $(KDIR) M=`pwd` $(CROSS) clean
````

```sh
# 编译内核
philon@a55v:~/drivers/00-hello$ make
# 将内核模块ko远程复制到开发板
philon@a55v:~/drivers/00-hello$ scp hello.ko rpi.local:/home/philon/modules/

# 开发板加载hello模块
philon@rpi:~/modules $ sudo insmod hello.ko
# 卸载内核
philon@rpi:~/modules $ sudo rmmod hello
# 查看内核打印信息
philon@rpi:~/modules $ dmesg | tail -5
[ 1176.602268] Under-voltage detected! (0x00050005)
[ 1182.842326] Voltage normalised (0x00000000)
[ 4731.671023] hello: no symbol version for module_layout
[ 4731.671042] hello: loading out-of-tree module taints kernel.
[ 4731.672307] hello kernel  👈看到没，就是它了！
[ 5010.908453] bye kernel
```
