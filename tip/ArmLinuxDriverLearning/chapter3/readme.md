# 嵌入式Linux驱动学习三：rootfs启动过程详解

话说上回：根文件系统的`dev proc sys`三个目录并非普通的目录，它们都是不同的文件系统，并不挂载到硬盘分区，而是由内核专门管理的内存区域，便于以文件的形式让用户层与内核层的交互。而由于我们第一节定制的最小Linux操作系统是个“阉割版”，并不存在`proc sys`两个目录，而且`dev`目录实质上也是手动创建的，随根目录直接挂载到硬盘，终归不符合“江湖规矩”。

因此为了之后驱动开发做铺垫，本章节主要说明Linux文件系统的启动顺序，解释清楚rootfs加载之后是如何一步步把这些特殊的目录挂载到与之对应的文件系统当中的。所以本回算是一个番外篇！

## 根文件系统初始化

我们都知道嵌入式操作系统的启动步骤是这样的：u-boot --> kernel --> rootfs  
但rootfs被加载之后还有很繁杂的过程，是这样的： rootfs --> /sbin/init --> /etc/inittab --> /etc/rcN.d --> /etc/init.d/rcS

- `init`是系统启动后的第一个程序，它的进程ID为1，之后的进程都是它的后代，
- `inittab`是初始化表，相当于init的配置文件，根据不同的运行级别执行不同的脚本或命令
- `rcN.d`是不同运行级别的初始化“功能清单”，配合inittab使用
- `rcS`是初始化脚本

### 关于Linux运行级别

运行级别在嵌入式Linux和PC-Linux中的区别是很大的，因为PC-Linux一般是跑在服务器上，多用户、升级、维护、恢复等场景，面对众多的后台服务，每种场景都需要定制不同的启动策略，所以才孕育除了“运行级别”这个概念。嵌入式Linux(或者说busybox版文件系统)就没那么复杂了，智能终端往往不会有多用户和维护的场景，如果死机了，就用锅铲拍一拍，所以**busybox保留了Linux运行级别的机制，但实际上并没有使用**。

为了之后讲清楚`inittab语法`，了解一点运行级别的知识也不坏。

Linux下有无数的服务要跑，称之为守护进程(Daemon)，比如`httpd sshd ftpd`等等，正常情况下我们希望这些服务能开机启动，所以网上有很多教程都让你把要开机启动的命令写到`/etc/init.d/rcS`文件中。这个固然没错，但忽略了一点，该脚本是“无脑”加载的，假如我们要对系统进行恢复出厂设置，那么前面的三个服务在系统重启后根本没必要运行了；有时候为了排查系统bug，启动一堆多余的服务反而造成障碍等。关于在不同的模式下执行不同的开机启动方案，就是Linux的**运行级别**。

所谓运行级别就是字面意思，在不同的场景下定制不同的后台服务，Linux一共7种运行级别(0-6)：

|  Level  |         Mode        |           Description        |
|:-------:|:-------------------:|------------------------------|
|    0    | 关机                 | 不要设置为initdefault动作      |
|    1    | 单用户模式            | root权限，常用于系统恢复/维护    |
|    2    | 多用户模式(停用网络)   | 登陆后进入命令行                |
|    3    | 多用户模式(启用网络)   | 登陆后进入命令行，最常见         |
|    4    | 未使用/预留           |                              |
|    5    | 图形界面模式          | 登陆后进入GUI界面，PC上最常见    |
|    6    | 重启                 | 不要设置为initdefault动作      |

可以通过`runlevel`命令查看自己的系统当前处于哪个运行级别：
```sh
$ runlevel 
N 5 # 表示当前运行在GUI模式
```

⚠️ 一般而言0、1、6的运行级别都是一样的，但2-5分别表示什么运行级别，不同的Linux发行版不一样。

### 关于inittab

关于inittab文件需要掌握的只有两点：

1. 它是init进程的配置文件，**PC-Linux**的inittab具体说明可参考👉[oracle官方手册](https://docs.oracle.com/cd/E19683-01/817-0716/6mggehbrk/index.html)
2. 它有自己的语法规则，`'<id>:<runlevels>:<action>:<process>'`表示`'唯一标示:运行级别:执行动作:命令程序'`

**(由于busybox中的init机制不同于PC-Linux，以下内容仅以嵌入式端busybox版本为主，具体参考👉[busybox inittab](https://git.busybox.net/busybox/plain/examples/inittab?h=1_8_stable))**


⚠️ **id**在busybox里并不作为标示，而是用于指定启动程序的tty，通常为缺省值  
⚠️ **runlevels**在busybox中被完全忽略，总是为缺省值

**执行动作\<action\>**

action表示以什么样的方式执行程序，有很多种动作可选：

|     Action    |                             Description                        |
|:-------------:|:---------------------------------------------------------------|
|    `sysinit`  | 系统初始化时触发，最先被执行                                        |
|    `respawn`  | 如果程序文件不存在，忽略它；如果程序挂了，重启它；如果程序已经在跑了，跳过它|
|   `askfirst`  | 相当于`respawn`，但执行命令前会先显示"Please press Enter to activate this console."，等到用户回车后才执行  |
|     `wait`    | 启动程序并等它运行结束，然后再执行之后的程序                          |
|     `once`    | 启动程序，不等待，死了也不管                                       |
|   `restart`   | 当收到重启信号时触发                                              |
|  `ctrlaltdel` | 当按下`'ctrl+alt+del'`组合键后触发                                |
|   `shutdown`  | 当关机时触发                                                     |

**命令程序\<process\>**

就是常规的命令行，如果有特殊要求，也可以通过`exec sh`来执行。

### 制作一个简单的inittab

```sh
# 系统初始化时，执行rcS脚本
::sysinit:/etc/init.d/rcS

# 如果用串口终端登陆，要求用户先按一下回车才能进入，并配置串口通信参数
::askfirst:/sbin/getty 115200 ttyS0

# 指定其他终端登陆的方式
tty2::askfirst:-/bin/sh
tty3::askfirst:-/bin/sh
tty4::askfirst:-/bin/sh

::restart:/sbin/init

# 关机要卸载有关驱动，停用交换空间
::shutdown:/bin/umount -a -r
::shutdown:/sbin/swapoff -a
```

从配置中的第2行就能看出，为什么我们需要开机启动的程序都必须卸载/etc/init.d/rcS这个地方了😄。rcS就是一个普通的shell脚本，想怎么写都行，达到目的即可。通常我们会在其中完成网络配置、各种后台服务启动、环境初始化等等。

## 文件系统自动挂载

初始化过程中，在应用程序启动之前，需要先确保Linux环境就绪，这其中就包括很底层的各种文件系统和驱动的挂载。设想一个网络摄像机，在开机后应用程序开始采集图像编解码并通过网络上传到服务端，结果发现/dev下根本没有设备文件，尝试用ps看一下进程列表，结果啥也没有，发现/proc目录是空的，会不会很崩溃，赶紧拿锅铲拍一拍😂。

因此，开机自动挂载文件系统是非常重要的，首先可以明确rootfs已经被内核自动挂载到根目录了，根据上一小节的学习可知，至少还有4个文件系统需要挂载，对应命令为

```sh
mount -t proc proc /proc    # 进程文件系统挂载
mount -t sysfs sysfs /sys   # 子系统文件系统挂载
mount -t tmpfs tmpfs /dev   # 设备描述符不要直接挂载到flash，避免频繁读写
mount -t tmpfs tmpfs /tmp   # 就是个块内存，掉电不保持，所以叫临时
```

我们当然可以把这些命令添加到`/etc/init.d/rcS`脚本，如果需要卸载时又一条条的umount，不觉得这样很麻烦么，有没有一种方便点的文件系统挂载方式。

### /etc/fstab文件解析

`fstab`翻译过来就是文件系统配置表，配合命令`mount -a`和`umount -a`使用，命令会根据配置表的内容逐条挂载文件系统到指定的挂载点。

fstab同样有自己的格式要求，一行内容表示一个文件系统的挂载，内容为：

```sh
# 设备或目标  挂载点        fs类型  选项       是否备份  是否检查
# device    mount-point  type   options    dump    pass
proc        /proc        proc   defaults    0       0
sysfs       /sys         sysfs  defaults    0       0
tmpfs       /dev         tmpfs   defaults   0       0
tmpfs       /tmp         tmpfs   defaults   0       0
```

如果/etc/fstab已存在，只需手动输入`mount -a`，就会根据配置自动把表里的文件系统挂载上。如果想开机自动挂载怎么办，还记得`/etc/inittab`么😁？添加两行：

```sh
::sysinit:/bin/mount -a       # 开机自动挂载
::shutdown:/bin/umount -a -r  # 关机自动卸载
```

## 完善varm的rootfs

明白了rootfs的启动过程以及其它文件系统的自动挂载原理，接下来就该完善之前做好的“阉割版”Linux了：

```sh
$ cd ~/varm/os/rootfs
$ mkdir -p etc/init.d proc sys dev tmp # 创建相关目录
$ vim etc/inittab # 创建init配置表
# 以下为inittab内容
#   ::sysinit:/bin/mount -a  
#   ::sysinit:/etc/init.d/rcS
#   ::askfirst:/bin/sh       
#   ::ctrlaltdel:/sbin/reboot
#   ::shutdown:/sbin/swapoff -a
#   ::shutdown:/bin/umount -a -r
#   ::restart:/sbin/init

$ vim etc/fstab # 创建自动挂载配置表
# 以下为fstab内容
#   proc        /proc        proc   defaults    0       0
#   sysfs       /sys         sysfs  defaults    0       0
#   tmpfs       /dev         tmpfs   defaults   0       0
#   tmpfs       /tmp         tmpfs   defaults   0       0

$ vim etc/init.d/rcS # 创建开机启动脚本
# 以下为rcS内容
#   #! /bin/sh
#   for i in 1 2 3 4
#   do
#       /bin/mknod /dev/tty$i c 4 $i
#   done
$ chmod +x etc/init.d/rcS # 脚本必须有“可执行”权限
```

完成rootfs的改造后，重新将其写入镜像文件中：

```sh
$ cd ~/varm/os
$ sudo mount -o loop rootfs.ext3 /mnt
$ sudo cp -r rootfs/* /mnt
$ sudo umount /mnt
```

## 小结一下

- 嵌入式Linux的文件系统启动顺序为init --> inittab --> fstab --> rcS
- inittab的基本语法为`'<id>:<runlevels>:<action>:<process>'`
- fstab的基本语法为`'device mount-point type options dump pass'`
- /etc/init.d/rcS就是一个普通脚本，开机启动的入口