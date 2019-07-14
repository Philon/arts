# 模块驱动，必须以obj-m=xxx形式编写
obj-m = hello.o

KDIR = /home/philon/linux-rpi-4.19.y
CROSS = ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-

all:
	$(MAKE) -C $(KDIR) M=$(PWD) $(CROSS) modules

clean:
	$(MAKE) -C $(KDIR) M=`pwd` $(CROSS) clean