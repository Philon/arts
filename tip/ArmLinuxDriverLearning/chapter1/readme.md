# 嵌入式Linux驱动学习一：环境准备

问：作为嵌入式软件工程师，你会写驱动吗？
答：会呀，我会控制gpio输出把LED点亮！😂

我想这是我扎根嵌入式软件开发多年的尴尬之处——不太会写驱动！所以，我决定系统地学习一下这个领域，一来是工作需要弥补我在驱动层的空白，二来自己也想全面了解Linux内核机制。基本上我主要通过《Linux设备驱动开发详解，基于最新的Linux4.0内核》以及《Linux设备驱动程序》作为主要参考教材(毕竟手上就这两本书)。

*废话结束*
***

为了避免繁琐地硬件环境搭建，学习过程尽可能基于虚拟的arm开发板，既能保证环境统一，又能快速上手👍。

另外，我个人的“战果”全部放在git仓库：[github/philon/varm](https://github.com/philon/varm)

**个人开发环境：**

- 宿主机：Ubuntu 18.04
- 开发板：qemu+vexpress-a9
- 编辑器：visual studio code
- 编译器：arm-linux-gnueabihf-gcc

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

选中自己喜欢的“硬件”环境后，理论上就可以“上电”了，不过且慢——**操作系统还没装呢**！所以接下来很重要的一步就是制作自己的Linux系统镜像。

## 制作Linux系统镜像

做嵌入式Linux的人都知道，一个完整的嵌入式Linux操作系统基本分为三大块：
- u-boot：上电初始化并引导内核
- kernel：Linux内核
- rootfs：根文件系统，busybox

由于qemu可以直接引导内核，所以这里暂且略过u-boot的移植。其它两个源码建议选择“稳定版”，别给自己找麻烦。

**第一步：移植内核**

从[linux内核官网](https://www.kernel.org)下载源码并解压(我嘞个去！Linux都步入5.0时代了，果断下载)，然后根据以下命令移植：

```sh
# 0. 可能需要安装flex、bsion，如果之前没装的话，否则编译镜像时会出错
$ sudo apt install flex bison

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

根据上述命令完成kernel移植之后，所有需要的镜像及其相关文件全部放在`<kernel_dir>/arch/arm/boot`中：
```sh
# zImage和dts就是真正需要的东西
~/varm/os/linux-5.0.3$ ls arch/arm/boot/
Image               bootp               deflate_xip_data.sh install.sh
Makefile            compressed          dts                 zImage

# 为了之后操作方便，我把它们放到varm/os目录下
~/varm/os/linux-5.0.3$ cp -rf arch/arm/boot/zImage .. # 内核镜像
~/varm/os/linux-5.0.3$ cp -f arch/arm/boot/dts/vexpress-v2p-ca9.dtb .. # 设备树描述
```

**第二步：启动内核**

既然有了内核镜像文件，就可以先小试牛刀了，qemu走起！

```sh
~/varm/os/linux-5.0.3$ cd ..
~/varm/os$ qemu-system-arm -M vexpress-a9 -m 512M -kernel zImage -dtb vexpress-v2p-ca9.dtb -nographic
```

![](https://i.loli.net/2019/03/23/5c96479700132.png)

可以看到kernel被成功启动了，但由于没有文件系统，内核向你抛出了一个异常。  
此外，上述命令比较长，简单解释下：
```sh
qemu-system-arm \ # 虚拟机启动
-M vexpress-a9 \ # 指定开发板为vexpress-a9
-m 512M \ # 配置虚拟机内存为512M
-kernel zImage \ # 指定内核镜像文件
-dtb vexpress-v2p-ca9.dtb \ # 指定设备树文件
-nographic \ # 不需要图形界面(LCD)
```

还有一点，quem启动后是一个独立进程，所有的Ctrl+C和其他中断信号都会被这个进程来接，程序无法关闭，最好的办法是新建一个终端，用kill来杀！
```sh
$ killall qemu-system-arm
# 也可以是ps先看qemu的pid，在用kill <pid>来杀，我只是觉得那样麻烦
```

**第三步：移植busybox**

同样先从[busybox官网](https://busybox.net)把源码包下载下来，然后开始移植：

```sh
# 1. 解压源码并进入目录
~/varm/os$ tar xf busybox-1.30.1.tar.bz2 && cd busybox-1.30.1

# 2. 选择默认配置
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- defconfig

# 3. 编译busybox
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j8

# 4. 安装busybox到./_install目录
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- install

# 顺利完成上述步骤后，可以在`busybox/_install`
# 目录下看到各种`usr lib bin`之类的文件系统结构
# 这就是rootfs的雏形了，现在还需要在此基础上做些加工

# 5. 为了方便之后操作，一样将_install目录放到varm/os下，并重命名为rootfs
~/varm/os/busybox-1.30.1$ mv _install ../rootfs
```

**第四步：制作根文件系统rootfs**

busybox已经生成了linux常用的一些命令和简单的目录结构，现在还差两个东西：
- busybox的命令执行是依赖于交叉编译工具的动态库的，所以要把动态库放入rootfs
- 需要给文件系统一些默认的设备描述符，否则你想让它往哪输出

```sh
# 1. 拷贝根文件系统的“必需品”到rootfs目录
~/varm/os/busybox-1.30.1$ cd ..
~/varm/os$ mkdir rootfs/lib # 创建系统库文件存放目录
~/varm/os$ cp -P /usr/arm-linux-gnueabihf/lib/*.so* rootfs/lib # 拷贝gcc动态库

# 2. 创建设备目录以及4个tty终端和调试串口
~/varm/os$ mkdir rootfs/dev # 创建设备描述符目录
~/varm/os$ for i in 1 2 3 4; do sudo mknod rootfs/dev/tty$i c 4 $i; done
```

**第五步：创建rootfs镜像**

所谓镜像可以理解为一张虚拟的光盘，存放操作系统。但玩过树莓派的同学应该都知道它的存储其实就是一个SD卡，那好，我们就创建一张虚拟的SD卡给qemu加载。

```sh
# 1. 生成镜像文件(虚拟SD卡)，且大小为32M
$ dd if=/dev/zero of=rootfs.ext3 bs=1M count=32 # 创建一个32M的空文件
$ mkfs.ext3 rootfs.ext3 # 将该文件格式化为ext3

# 2. 挂在镜像到/mnt目录，并将rootfs目录导入其中
$ sudo mount -o loop rootfs.ext3 /mnt
$ sudo cp -r rootfs/* /mnt
$ sudo umount /mnt
```

现在整个rootfs.ext3镜像制作完成，再加上zImage内核镜像，~~可以起飞了~~可以开机正常加载了，重新调整一下qemu的启动命令：
```sh
~/varm/os$ qemu-system-arm -M vexpress-a9 -m 512M -kernel zImage -dtb vexpress-v2p-ca9.dtb -sd rootfs.ext3 -nographic -append "root=/dev/mmcblk0 console=ttyAMA0"
```

上边的命令大体上和内核启动时一样，主要是增加了`-sd rootfs.ext3`文件系统的SD卡和对应分区，以确保内核能正确加载文件系统。经过一分钟左右的等待，我们的最小Linux系统成功运行起来了😄：

![](https://i.loli.net/2019/03/23/5c9649750ea24.png)

如果顺利的话内核会成功挂在文件系统，从此可以愉快玩耍了。以上就是整个ArmLinux的虚拟机搭建过程，目前位置整个环境基本OK，但在真正写代码之前还有些事情要做，总之等遇到了再查缺补漏。

## 小结一下

- QEMU是一款虚拟机，可以虚拟常见的通用处理器架构，包括ARM，支持很多开发板模拟
- vexpress-a9是ARM官方出的一款开发板
- QEMU可以直接引导内核，因此可以不用移植u-boot