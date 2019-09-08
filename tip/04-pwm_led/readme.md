# 树莓派驱动开发实战04：PWM呼吸灯

本文源码：https://github.com/Philon/rpi-drivers/tree/master/04-pwm_led

如果你对硬件了解不是很深，看了标题可能会一脸懵逼，PWM是什么？别急，在回答这个问题之前，先看看PWM+一个普通的LED灯能实现什么效果：

![pwmled.gif](https://i.loli.net/2019/09/08/zYci2Z4kymaEbho.gif)

(动图有点大，请稍等...)

从结果来看可能第一反应是这样的，普通的LED灯只能控制开/关，既然PWM可以控制LED的亮和暗，那它应该是一种电压控制器！其实根本不是那么回事，和电压毛关系都没有。它背后的原理恰恰是它最有意思的地方，它的本领可不仅仅是拿来控制个灯的明暗那么low。

## PWM基础

PWM——脉冲宽度调制，顾名思义，就是一个脉冲信号输出源，且方波的周期(period)以及高电平持续时间(duty)是可调的。从软件角度来看，主要关注两个地方：

- `period`，脉冲周期，就是发送一次1和0交替的完整时间
- `duty`，占空比，就是一个脉冲周期内1占了多长时间。

有时候，还需要关注1秒钟发送多少个脉冲信号，即频率，不过对于本文要控制的led灯亮度而言，频率几乎可以忽略。关于PWM原理，如果还是云里雾里，就看一下这个视频介绍，非常简单。

不过先提醒以下，PWM的实际应用非常广，有红外遥控、音频控制、通信等，最常见的要数直流电机控制，以及手机上的背光灯亮度。总之，掌握pwm的原理及应用是灰常有必要滴。

<iframe src="https://player.bilibili.com/player.html?aid=10356317&cid=17106116&page=1" scrolling="no" border="0" frameborder="no" framespacing="0" allowfullscreen="true" style="width:480px;height:320px"> </iframe>

## 树莓派上的PWM

树莓派上的PWM比较坑，明明很简单的东西我调了两天，网上99%的教程都是各种应用层的脚本或writingPi的函数调用，和Linux内核的边都沾不上，而且其中绝大多数还是相互复制粘贴。所以真正有用的资料寥寥无几，一直卡在`pwm_request`却获取不到pwm资源，当然最主要原因还是我对技术细节的不理解，好在发现了一个老外的博客：https://jumpnowtek.com/rpi/Using-the-Raspberry-Pi-Hardware-PWM-timers.html

在官方的[《BCM2835外围指南》](https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2835/BCM2835-ARM-Peripherals.pdf)第9章说得很清楚，处理器内部集成了两个独立的PWM控制器，理论上可以达到100MHz的脉冲频率。**树莓派扩展接口共有4个GPIO引出PWM**，具体为：

|PWM通道| GPIO号 |物理引脚|  复用功能  |
|------|--------|------|-----------|
| PWM0 | GPIO12 |  32  | Alt Fun 0 |
| PWM1 | GPIO13 |  33  | Alt Fun 0 |
| PWM0 | GPIO18 |  12  | Alt Fun 5 |
| PWM1 | GPIO19 |  35  | Alt Fun 5 |

**第一步，启用pwm（默认情况下未启用）**

简而言之，你无法通过Linux内核API获取到PWM资源，因为在树莓派官方的设备树配置(`/boot/config.txt`)里并没有通知内核要启用pwm。因此第一步自然是让内核支持pwm驱动，使用如下命令：

```sh
# vim打开/boot/config.txt
# 在最后一行加入： dtoverlay=pwm
# 保存退出，重启

philon@rpi:~ $ sudo vim /boot/config.txt 
philon@rpi:~ $ sudo reboot

# 重启之后，有两种方式确认pwm已启用
philon@rpi:~ $ lsmod | grep pwm
pwm_bcm2835            16384  1 # 方式1: 加载了官方pwm驱动

philon@rpi:~ $ ls /sys/class/pwm/
pwmchip0                        # 方式2: sysfs里可以看到pwmchip0目录
```

**第二步，搭建硬件环境**

非常简单，LED的正极拉到一个PWM通道，负极随便找一个GND接上。

![PWMLED接线图](https://i.loli.net/2019/09/08/fKURQTq8O6rgSAp.png)

如图所示，将三色LED的绿灯脚接到GPIO18，也就是PWM0通道，再随便找一个GND接上即可，和第一章的点亮一个LED一样，关键看软件如何操作了。

![PWMLED原理图](https://i.loli.net/2019/09/08/IYM7C91OXikftjy.png)


## 使用命令行控制PWM

根据之前的硬件接线，LED与树莓派的PWM0通道相连，所以使能pwm0即可点亮led，大体步骤为：

1. 请求pwm0资源
2. 设置脉冲周期
3. 设置占空比
4. 打开pwm0

命令行控制pwm其实和gpio大同小异，都是通过sysfs这个虚拟文件系统完成的。

```sh
philon@rpi:~ $ cd /sys/class/pwm/pwmchip0/    # 进入pwm资源目录

philon@rpi:~ $ echo 0 > export                # 加载pwm0资源
philon@rpi:~ $ echo 10000000 > pwm0/period    # 设置脉冲周期为10ms(100Hz)
philon@rpi:~ $ echo 8000000 > pwm0/duty_cycle # 设置占空比为8ms
philon@rpi:~ $ echo 1 > pwm0/enable           # 开始输出

# 可以自行调整脉冲周期和占空比，得到不同的亮度
# 如果玩够了，记得释放资源
philon@rpi:~ $ echo 0 > pwm0/enable           # 关闭输出
philon@rpi:~ $ echo 0 > unexport              # 卸载pwm0资源
```

经过上面这番犀利操作，只要你够虔诚，就会看见一束绿光闯入你的眼里，心里，脑海里。

## Linux驱动控制PWM

在实现pwm调光led之前，需要先交代清楚：

由于本驱动仅仅是调节led的亮度，关于pwm的`占空比`和`脉冲周期`两个参数，其实只调节占空比就够了。你想一下，脉冲周期长短，无非就是led闪烁频率，只要人眼看着不闪，再快有个毛用。因此本驱动将脉冲周期固定为1ms，即1KHz，而占空比的取值为0~1000us，说白了，就是led从最暗到最亮一共分为一千个档位。

内核为pwm提供了标准的API接口，需要掌握这几个：

```c
// PWM channel object
struct pwm_device {
	const char *label;      // name of the PWM device
	unsigned long flags;    // flags associated with the PWM device
	unsigned int hwpwm;     // per-chip relative index of the PWM device
	unsigned int pwm;       // global index of the PWM device
	struct pwm_chip *chip;  // PWM chip providing this PWM device
	void *chip_data;        // chip-private data associated with the PWM device
	struct pwm_args args;   // PWM arguments
	struct pwm_state state; // curent PWM channel state
};

/**
 * 通过pwm通道号获取pwm通道对象
 * @pwm_id    通道号
 * @label     pwm通道别名
 */
struct pwm_device *pwm_request(int pwm_id, const char *label);

/**
 * 释放pwm通道对象
 */
void pwm_free(struct pwm_device *pwm);

/**
 * 设置pwm通道的相关参数
 * @duty_ns     以纳秒为单位的占空比
 * @period_ns   以纳秒为单位的脉冲周期
 */
int pwm_config(struct pwm_device *pwm, int duty_ns, int period_ns)

/**
 * 打开pwm通道，开始输出脉冲
 */
int pwm_enable(struct pwm_device *pwm)

/**
 * 关闭pwm通道，停止输出脉冲
 */
void pwm_disable(struct pwm_device *pwm)
```

接着，实现pwmled的驱动吧：
```c
/********************* pwmled.h *********************/

#define PWMLED_MAX_BRIGHTNESS 1000

typedef enum {
  PWMLED_CMD_SET_BRIGHTNESS = 0x1,
  PWMLED_CMD_GET_BRIGHTNESS,
} pwmled_cmd_t;

/********************* pwmled.c *********************/

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/pwm.h>

#include "pwmled.h"

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Phlon | https://ixx.life");

#define PWMLED_PERIOD 1000000 // 脉冲周期固定为1ms

static struct {
  struct pwm_device* pwm;
  unsigned int brightness;
} pwmled;

long pwmled_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {
  switch (cmd) {
  case PWMLED_CMD_SET_BRIGHTNESS:
    // 所谓调节亮度，就是配置占空比，然后使能pwm0
    pwmled.brightness = arg < PWMLED_MAX_BRIGHTNESS ? arg : PWMLED_MAX_BRIGHTNESS;
    pwm_config(pwmled.pwm, pwmled.brightness * 1000, PWMLED_PERIOD);
    if (pwmled.brightness > 0) {
      pwm_enable(pwmled.pwm);
    } else {
      pwm_disable(pwmled.pwm);
    }
  case PWMLED_CMD_GET_BRIGHTNESS:
    return pwmled.brightness;
  default:
    return -EINVAL;
  }

  return pwmled.brightness;
}

static struct file_operations fops = {
  .owner = THIS_MODULE,
  .unlocked_ioctl = pwmled_ioctl,
};

static struct miscdevice dev = {
  .minor = 0,
  .name = "pwmled",
  .fops = &fops,
  .nodename = "pwmled",
  .mode = 0666,
};

int __init pwmled_init(void) {
  // 请求PWM0通道
  struct pwm_device* pwm = pwm_request(0, "pwm0");
  if (IS_ERR_OR_NULL(pwm)) {
    printk(KERN_ERR "failed to request pwm\n");
    return PTR_ERR(pwm);
  }

  pwmled.pwm = pwm;
  pwmled.brightness = 0;

  misc_register(&dev);

  return 0;
}
module_init(pwmled_init);

void __exit pwmled_exit(void) {
  misc_deregister(&dev);
  // 停止并释放PWM0通道
  pwm_disable(pwmled.pwm);
  pwm_free(pwmled.pwm);
}
module_exit(pwmled_exit);
```

该驱动会自动创建`/dev/pwmled`设备节点，然后应用层通过ioctl即可设置和获取灯的亮度等级。

```c
int main(int argc, char* argv[]) {
  int fd = open("/dev/pwmled", O_RDWR);
  int brightness = 0;
  char key = 0;

  while ((key = getchar()) != 'q') {
    switch (key) {
    case '=':
      brightness += brightness < PWMLED_MAX_BRIGHTNESS ? 10 : 0;
      break;
    case '-':
      brightness -= brightness > 0 ? 10 : 0;
      break;
    }

    if (ioctl(fd, PWMLED_CMD_SET_BRIGHTNESS, brightness) < 0) {
      perror("ioctl");
      break;
    }
  }

  close(fd);
  return 0;
}
```

正如本文开头那张动图一样，用户只需要按`+/-`即可调节led的亮度，so easy~

## 小结

- PWM即脉冲宽度调制，可以实时改变脉冲源的占空比和周期时长
- 树莓派Linux内核默认不加载pwm通道，需要在/boot/config.txt中增加一行dtoverlay=pwm
- pwm可以像gpio一样通过命令行对sysfs虚拟文件系统操作，即可控制pwm资源
- 内核提供了标准的pwm接口，注意通道值的配置是以纳秒为单位
- pwm技术应用范围非常广，务必牢牢掌握