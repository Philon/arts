# 嵌入式Linux驱动学习一：环境准备

问：作为嵌入式软件工程师，你会写驱动吗？
答：会呀，我会控制gpio输出把LED点亮！😂

我想这是我扎根嵌入式软件开发多年的尴尬之处——不太会写驱动！所以，我决定系统地学习一下这个领域，一来是工作需要弥补我在驱动层的空白，二来自己也想全面了解Linux内核机制。基本上我主要通过《Linux设备驱动开发详解，基于最新的Linux4.0内核》以及《Linux设备驱动程序》作为主要参考教材(毕竟手上就这两本书)。

*废话结束*
***

为了避免繁琐地硬件环境搭建，学习过程尽可能基于虚拟的arm开发板，既能保证环境统一，又能快速上手👍。

另外，我个人的“战果”全部放在git仓库：[github/philon/varm]()

**个人开发环境：**

- 宿主机：macOS(如果是Windows平台，建议上virtualbox+ubuntu)
- 开发板：qemu+vexpress-a9
- 编辑器：visual studio code
- 编译器：arm-linux-gnueabi-gcc

关于如何安装Linux虚拟机以及交叉编译环境的搭建这里就不写了，网上教程一大堆。所以从现在开始，阅读下文的前提是：Linux、arm-linux-gcc、各种源码编辑器环境已就绪。

好，下面亲手动手，来做一块arm开发板😅。

## ARM虚拟机——QEMU

[QEMU](https://www.qemu.org)是一款免费开源且跨平台的虚拟机，可以虚拟各种架构的处理器，和vmware/virtualbox不同，我们更喜欢用它来仿真ARM环境，即一款ARM虚拟机。

**第一步：下载并安装qemu**

```sh
# macOS用户
$ brew install qemu

# Ubuntu用户
$ sudo apt-get install qemu

# Windows用户麻烦把门口的小黄车挪一挪
```

**第二步：选择一款arm**

装好后可以通过以下命令qemu支持哪些arm处理器(开发板)：
```sh
$ qemu-system-arm -M help
akita                Sharp SL-C1000 (Akita) PDA (PXA270)
ast2500-evb          Aspeed AST2500 EVB (ARM1176)
borzoi               Sharp SL-C3100 (Borzoi) PDA (PXA270)
...
vexpress-a15         ARM Versatile Express for Cortex-A15
vexpress-a9          ARM Versatile Express for Cortex-A9 # 👈没错就它了
...
witherspoon-bmc      OpenPOWER Witherspoon BMC (ARM1176)
xilinx-zynq-a9       Xilinx Zynq Platform Baseboard for Cortex-A9
z2                   Zipit Z2 (PXA27x)
```

选中自己喜欢的“硬件”环境后，理论上就可以“上电”了，不过且慢——操作系统还没装呢！所以接下来很重要的一步就是制作自己的Linux系统镜像。

## 制作Linux系统镜像

做嵌入式Linux的人都知道，一个完整的嵌入式Linux操作系统基本分为三大块：
- u-boot：上电初始化并引导内核
- kernel：Linux内核
- rootfs：根文件系统，busybox

由于qemu可以直接引导内核，所以这里暂且略过u-boot的移植。其它两个源码建议选择“稳定版”，别给自己找麻烦。

**第一步：移植内核**

从[linux内核官网](https://www.kernel.org)下载源码并解压(我嘞个去！Linux都步入5.0时代了，果断下载)，然后根据以下命令移植：

```sh
# 1. 解压源码并进入目录
~/varm/os$ tar xf linux-5.0.3.tar.xz && cd linux-5.0.3

# 2. 将内核设置为vexpress的配置，即vexpress-a9开发板
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- vexpress_defconfig

# 3. 编译内核镜像zImage
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage -j8

# 4. 编译其它驱动模块，制作rootfs时候要用
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules -j8

# 5. 编译设备树
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- dtbs -j8
```

macOS平台编译内核是可能会遇到`'elf.h' file not found`的情况，其原因是macOS系统没有elf.h这个文件，可以从[这里](https://www.rockbox.org/tracker/9006?getfile=16683)下载一个elf.h的源码并放到内核工程的`scripts`目录下。然后将提示该错误的所有文件(如scripts/mod/mk_elfconfig.c)的`#include <elf.h>`改为`#include "elf.h"`即可。说白了就是强行指定头文件所在位置。

根据上述命令完成kernel移植之后，所有需要的镜像及其相关文件全部放在`<kernel_dir>/arch/arm/boot`中：
```sh
# zImage和dts就是真正需要的东西
~/varm/os/linux-5.0.3$ ls arch/arm/boot/
Image               bootp               deflate_xip_data.sh install.sh
Makefile            compressed          dts                 zImage
```

**第二步：启动内核**

既然有了内核镜像文件，就可以先小试牛刀了，qemu走起！

```sh
$ qemu-system-arm -M vexpress-a9 -m 512M -kernel arch/arm/boot/zImage -dtb arch/arm/boot/dts/vexpress-v2p-ca9.dtb -nographic -append "console=ttyAMA0"

Booting Linux on physical CPU 0x0
Linux version 5.0.3 (philon@LMac.local) (gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)) #1 SMP Wed Mar 20 21:47:37 CST 2019
CPU: ARMv7 Processor [410fc090] revision 0 (ARMv7), cr=10c5387d
...
```

可以看到kernel被成功启动了！不过上述命令比较长，简单解释下：
```sh
$ qemu-system-arm \
-M vexpress-a9 \ # 指定开发板为vexpress-a9
-m 512M \ # 配置虚拟机内存为512M
-kernel arch/arm/boot/zImage \ # 指定内核镜像位置
-dtb arch/arm/boot/dts/vexpress-v2p-ca9.dtb \ # 指定设备树位置
-nographic \ # 不需要图形界面(LCD)
-append "console=ttyAMA0" # 串口0作为终端输出
```

**第三步：移植busybox**

```sh
# 1. 选择默认配置
$ make ARCH=arm CROSS_COMPILE=arm-mac-linux-gnueabihf- defconfig

# 2. 编译busybox
$ make ARCH=arm CROSS_COMPILE=arm-mac-linux-gnueabihf-

# 3. 安装busybox到./_install目录
$ make ARCH=arm CROSS_COMPILE=arm-mac-linux-gnueabihf- install

```

顺利完成上述步骤后，可以在`busybox/_install`目录下看到各种`usr lib bin`之类的文件系统结构，这就是rootfs的雏形了，现在还需要在此基础上做些加工。

**第四步：制作根文件系统rootfs**

```sh
# 1. 新建一个rootfs目录，存放根文件系统的所有数据
$ mkdir -p rootfs

# 2. 拷贝根文件系统的“必需品”到rootfs目录
$ cp -f busybox/_install/* rootfs/ # busybox程序
$ cp -P /opt/arm-linux-gcc/lib/* rootfs/lib/ # gcc库

# 3. 创建4个tty终端和调试串口
sudo mknod rootfs/dev/tty1 c 4 1
sudo mknod rootfs/dev/tty2 c 4 2
sudo mknod rootfs/dev/tty3 c 4 3
sudo mknod rootfs/dev/tty4 c 4 4
```

**第五步：创建rootfs镜像**

所谓镜像可以理解为一张虚拟的光盘，存放操作系统。但玩过树莓派的同学应该都知道它的存储其实就是一个SD卡，那好，我们就创建一张虚拟的SD卡给qemu加载。

```sh
# 1. 生成镜像文件(虚拟SD卡)，且大小为32M
$ dd if=/dev/zero of=rootfs.ext3 bs=1M count=32
$ mkfs.ex3 rootfs.ext3

# 2. 挂在镜像，并将rootfs目录导入其中
$ sudo mount -t ext3 -o loop rootfs.ext3 /mnt
$ sudo cp -r rootfs/* /mnt
$ sudo umount
```

现在整个rootfs.ex3镜像制作完成，再加上zImage内核镜像，~~可以起飞了~~可以开机正常加载了，重新调整一下qemu的启动命令：
```sh
~/varm/os$ qemu-system-arm -M vexpress-a9 -m 512M -kernel zImage -dtb vexpress-v2p-ca9.dtb -nographic -append "init=/linuxrc root=/dev/mmcblk0p1 rw rootwait earlyprintk console=ttyAMA0"
```
