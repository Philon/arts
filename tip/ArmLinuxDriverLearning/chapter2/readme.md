# 嵌入式Linux驱动学习二：HelloWorld

采用QEMU制作的最小linux顺利启动后，就该为其写内核模块了，所以这一节就是热热身：
- 学习如何通过vscode搭建linux驱动开发环境
- 编写第一个内核模块helloworld
- 初始linux内核模块机制

警告：在开始阅读下文之前，务必**确保自己的linux内核被交叉编译过**，并顺利生成zImage。

## Ubuntu+VSCode的驱动开发环境搭建

2015年底微软用自己的行动向世人证明，在IDE领域，你大爷还是你大爷，即便是[Visual Studio Code](https://code.visualstudio.com)这种所谓的代码编辑器，都可以傲视一堆所谓功能强大的集成开发环境。随着近几年的更新迭代，vscode在c/c++的开发方面也不逊色于vs了(我个人感受)，在GO、Web、C/C++以及很多非主力语言，它都是我的首选。因此，之后的内核模块代码编写，自然也少不了这利器。(一不小心闲话扯多了)

进入正题，vscode安装好之后，务必安装微软官方扩展——[C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)，这个扩展的好处是提供语法高亮、智能补全、提示、调试等核心功能，剩下的酷炫功能及教程自行看官网。对于驱动来说，主要是方便代码补全和接口提示，以提高编写效率。

![](https://i.loli.net/2019/03/25/5c98e40cd2b1b.png)

vscode及其扩展安装完毕后，开始写代码吧，创建第一个驱动源码——hello.c
```sh
$ mkdir ~/varm/drivers/hello
$ cd ~/varm/drivers/hello
$ touch hello.c
```

先别急着写代码，vscode扩展需要我们告诉它从哪里解析头文件，以分析和自动补全你想要的接口形式，所以：
```sh
~/varm/drivers/hello$ mkdir .vscode # 创建vscode当前项目配置目录
~/varm/drivers/hello$ touch .vscode/c_cpp_properties.json # 创建C/C++扩展的配置
```

打开刚创建的json配置文件，并把下方的内容拷贝进取，不过要注意`includePath`部分，里边的路径是我自己的环境，说白了就是要告诉vscode从哪里能够**找到linux内核以及arm架构所需的头文件位置**，这个需要根据自己的实际情况作调整。

```json
{
    "configurations": [
        {
            "name": "Linux",
            "includePath": [
                "${workspaceFolder}/**",
                "/opt/varm/linux-5.0.3/include",
                "/opt/varm/linux-5.0.3/arch/arm/include",
                "/opt/varm/linux-5.0.3/arch/arm/include/generated",
                "/opt/varm/linux-5.0.3/include/uapi"
            ],
            "defines": [
                "__KERNEL__",
                "__GNUC__"
            ],
            "compilerPath": "/usr/bin/gcc",
            "cStandard": "c11",
            "cppStandard": "c++17",
            "intelliSenseMode": "clang-x64"
        }
    ],
    "version": 4
}
```

好了，现在可以愉快地写代码了，就像下图这样，vscode驱动开发环境配置结束。

![](https://i.loli.net/2019/03/25/5c98ef6a9633b.gif)

## 第一个linux内核驱动

先看代码！

```c
#include <linux/init.h>
#include <linux/module.h>

// 内核加载时触发
static int __init hello_init(void)
{
    printk("HELLO_MODULE: hello world\n");
    return 0;
}

// 内核卸载时触发
static void __exit hello_exit(void)
{
    printk("HELLO_MODULE: goodbye\n");
}

// 真正向内核制定该模块出入口
module_init(hello_init);
module_exit(hello_exit);

// 各种版权、扩展信息声明，无关紧要
MODULE_LICENSE("GPL v2"); // 开源许可证
MODULE_DESCRIPTION("my first kernel module"); // 模块描述
MODULE_ALIAS("HelloWorld"); // 模块别名
MODULE_AUTHOR("Philon"); // 模块作者
```

上述代码可以说非常简单了，我们定义了两个函数`hello_init/hello_exit`都只是简单打印了一句话，并通过`module_init/module_exit`分别告诉内核，当加载/卸载该模块的时候执行对应的函数。

**为什么不用printf函数打印**

一是因为printf是C的标准库函数，依赖于C的动态库文件，比如/lib/libc.so.6文件，内核就不应该直接去访问这个文件。  
二是因为模块是运行在内核层，而非用户层，printf要通过用户文件系统把内容输出到文件，同样是第一条理由，所以printk也就是指通过内核输出。

不光printk，内核开发中还会遇到很多k结尾的函数，比如mallock，都是因为标准C库的原因，不能直接调用，但其功能及接口形式与标准库几乎一样。

**两个函数前__init/__exit意味着什么**

这两个是修师符，即使不添加也不会有任何表面上的区别，但建议加上！

`__init`表示该函数是初始化函数，在最终生成的模块文件中，这段代码会被强制放到.init.text段区，当模块加载完毕后，该区所有分配的内存资源都会被释放。
`__exit`表示该函数是退出函数，如果你的驱动并非“模块”，而是直接编译进内核，那么被此修饰的函数显然是多余的(编译到内核中的驱动是无法卸载的)，因此该函数不会被链接，以缩小镜像大小。

**关于开源与许可证**

这部分会涉及到非常多的法律问题和风险，一般来说如果一个驱动没有声明GPL协议，在加载时会收到内核被污染的警告，逼死强迫症。但是对于我等初学小白而言，可以暂时不用考虑这部分内容。但如果是工作中的代码或涉及商业机密，建议还是谨慎对待，毕竟不是每个程序员都善于打官司。

## 编译并“运行”驱动

**第一步：编译**

linux的内核模块编译是必须依赖内核源码的，这就是为什么文章一开始注明必须确保linux内核已经交叉编译过的原因。而内核模块需要通过Makefile来指定是编译到内核中，还是以模块形式存在，也就是下面的第一行`obj-m=hello.o`。

```makefile
# 模块驱动，必须以obj-m=xxx形式编写
obj-m = hello.o

# 指定内核源码目录及交叉编译环境
KDIR = /opt/varm/linux-5.0.3
CROSS = ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-

all:
	$(MAKE) -C $(KDIR) M=`pwd` $(CROSS) modules

clean:
	$(MAKE) -C $(KDIR) M=`pwd` $(CROSS) clean
```

除了第一行需要留意下以外，其他都是常规Makefile语法，本文主要关注驱动开发，其他语法规则什么的就不废话了。直接make以下就能看到`hello.ko`的出现，这就是最终内核模块。

**第二步：拷入根文件系统**

虽然linux内核模块是运行在内核层的，但其加载、卸载、访问、操作等**策略**性的事物是完全交由用户层来管理，驱动仅仅负责实现和提供设备访问**机制**。所以现在需要把编译好的ko文件拷贝到之前制作好的根文件系统镜像中——rootfs.ext3。

目前我们的linux最小系统还很简陋，没有网络共享，因此最粗暴的方式就是挂载镜像——复制——启动系统。
```sh
$ sudo mount -o loop ~/varm/os/rootfs.ext3 /mnt
$ sudo cp ~/varm/drivers/hello/hello.ko /mnt
$ cd ~/varm/os && ./power_on # 完成拷贝，启动系统
```

**第三步：加载/卸载**

记住linux最简单的管理内核命令：
- insmod，加载内核模块
- rmmod，卸载内核模块

正如下图所示，hello.ko和预期一样分别在加载和卸载模块时打印出了相应的内容。

![](https://i.loli.net/2019/03/25/5c98fa2533f3b.png)

关于提示loading out-of-tree module taints kernel.主要是由于该模块并不存在设备树中，想想第一节的make dtbs，所以这个警告无关紧要。关键是**第一个内核模块版本的HelloWorld顺利通过验收**，鼓掌！