# 模块驱动，必须以obj-m=xxx形式编写
obj-m = rgbled.o

KDIR = /home/philon/linux-rpi-4.19.y
CROSS = ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-

TEST := rgbled_test
TEST_SRC := $(TEST).c
TEST_OBJ := $(TEST).o

all: module test

module:
	$(MAKE) -C $(KDIR) M=$(PWD) $(CROSS) modules

test: 
	arm-linux-gnueabihf-gcc $(CFLAGS) -c $(TEST_SRC) -o $(TEST_OBJ)
	arm-linux-gnueabihf-gcc $(LFLAGS) -o $(TEST) $(TEST_OBJ)

clean:
	$(MAKE) -C $(KDIR) M=`pwd` $(CROSS) clean
	rm -f $(TEST_OBJ) $(TEST)