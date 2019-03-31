# 模块驱动，必须以这种形式编写
obj-m = hello.o

KDIR = /opt/varm/linux-5.0.3
CROSS = ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-

all:
	$(MAKE) -C $(KDIR) M=`pwd` $(CROSS) modules

clean:
	$(MAKE) -C $(KDIR) M=`pwd` $(CROSS) clean