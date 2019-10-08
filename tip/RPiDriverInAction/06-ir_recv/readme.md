# 树莓派驱动开发实战06：红外接收

本文源码：https://github.com/Philon/rpi-drivers/tree/master/06-infrared

由于我手上只有一个1838红外接收头和一个CAR-MP3遥控器，所以本文主要基于Linux内核实现红外NEC协议的解码。

先来看看效果：

<iframe src="//player.bilibili.com/player.html?aid=70504618&cid=122149105&page=1" scrolling="no" border="0" frameborder="no" framespacing="0" allowfullscreen="true" style="width:600px;height:400px"> </iframe>

## 红外通信原理

呐，太专业的电路原理呢我就不展开讲了，反正也没人看。简单点说吧：

反正就是有一对红外发射管和接收管组成，通过产生脉冲信号来传递信息。脉冲信号是什么？你可以理解为摩尔斯电码那种样子，就是1和0。

红外通信在日常生活中主要应用于家电控制，例如电视、空调、投影等等。市面上比较常见的红外通信协议是NEC，所以就来研究以下NEC的解码。


## NEC协议

在讲述NEC协议之前，先来看看下面这几行数据打印。这是我随便按了几下遥控器，抓取的红外原始数据。“横杠”表示有红外信号，“下划线”表示无信号。

```sh
philon@rpi:~/modules $ dmesg
#                   9ms           4.5ms  0 0 0 0 0 0 0 0 1   1   1   1   1   1   1   1   0 0 0 1   0 0 0 0 1   1   1   0 1   1   1   1   1
[  203.718032] -----------------_________-_-_-_-_-_-_-_-_-___-___-___-___-___-___-___-___-_-_-_-___-_-_-_-_-___-___-___-_-___-___-___-___-_
[  207.647870] -----------------_________-_-_-_-_-_-_-_-_-___-___-___-___-___-___-___-___-_-_-___-___-___-_-_-_-___-___-_-_-_-___-___-___-_
[  209.927802] -----------------_________-_-_-_-_-_-_-_-_-___-___-___-___-___-___-___-___-_-___-_-___-___-_-___-_-___-_-___-_-_-___-_-___-_
[  214.557679] -----------------_________-_-_-_-_-_-_-_-_-___-___-___-___-___-___-___-___-_-___-_-_-_-_-___-_-___-_-___-___-___-___-_-___-_
[  216.917629] -----------------_________-_-_-_-_-_-_-_-_-___-___-___-___-___-___-___-___-_-___-_-_-___-_-___-_-___-_-___-___-_-___-_-___-_
[  219.457571] -----------------_________-_-_-_-_-_-_-_-_-___-___-___-___-___-___-___-___-_-___-_-___-_-_-___-_-___-_-___-_-___-___-_-___-_
```

从上边的原始数据可以看出来，每个NEC红外协议很相似，以9毫秒的高电平、4.5毫秒的低电平开始，之后跟上一堆1和0，最后一部分才是不相同的地方。简直可以总结出NEC的协议格式是这样的：

`<帧头9ms高+4.5低><8位地址码><8位地址码取反><8位指令码><8位指令码取反>`

没错，就是这样的😁，不过需要注意，**NEC协议采用PWM(脉宽调制)编码，一个脉冲周期表示一个bit，是0还是1取决于占空比**。不信请看下图：

![NEC协议编码说明](https://i.loli.net/2019/10/08/BQPUXCGspiTumE6.jpg)

⚠️我在程序中对接收到的数据取反，所以原始数据和上图的逻辑刚好相反。

结合原始数据和图片可以总结出：

1. 协议帧头总是以9ms的高电平和4.5ms的低电平为一个脉冲周期
2. 协议内容的脉冲周期，‘`-___`’表示1，‘`-_`’表示0，且电平信号以560us为单位；
3. 9ms高电平和2.25ms的低电平表示重复码，即长按按键时触发
4. 帧间间隔为110ms

## 红外接收电路

![红外接收管树莓派接线图](https://i.loli.net/2019/10/08/aLufX8MJltyDCpF.png)

如上图所示，红外接收管从左到右一共3个脚，分别是：地、3.3V、数据输出。所以供电就用树莓派自身的3.3V即可，而数据输出脚，我这里接的是GPIO18。

## 驱动实现

正如前文所述，NEC红外协议是高频脉冲信号，所以我用GPIO的中断来记录每一次脉冲信号及其时长。实现起来没什么太复杂的地方，大致流程为：

1. 申请并注册GPIO18的中断，务必是双边沿触发
2. 申请一个定时器用于超时断帧处理
3. 每次中断触发，都记录上升或者下降沿的状态及时长
4. 每当经过一个完整脉冲后，通过占空比判断数据类型
5. 每当记录了32个数据(一帧)后，处理协议指令
6. 我是直接把地址和指令推给用户层处理

⚠️注意：以下代码有个很大的风险，为了简化程序，IRQ中断我并没有采取“底半部”来处理复杂的红外解码业务，如果业务逻辑进一步加大，可能会导致内核崩溃。

```c
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/wait.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Philon | https://ixx.life");

static struct {
  int gpio;
  int irq;
  wait_queue_head_t rwait;
  struct timer_list timer;
  u32     pulse;  // 脉冲上升沿持续时长
  u32     space;  // 脉冲下降沿持续时长
  size_t  count;  // 脉冲个数
  u32     data;   // 脉冲解码后的值
} ir;

#define is_head(p, s) (p > 8900 && p < 9100 && s > 4400 && s < 4600)
#define is_repeat(p, s) (p > 8900 && p < 9100 && s > 2150 && s < 2350)
#define is_bfalse(p, s) (p > 500 && p < 650 && s > 500 && s < 650)
#define is_btrue(p, s) (p > 500 && p < 650 && s > 1500 && s < 1750)

// 红外接收函数(即GPIO18的双边沿中断处理函数)
// 记录GPIO每次中断是“上升还是下降”，以及持续的时长
static irqreturn_t ir_rx(int irq, void* dev) {
  static ktime_t last = 0;
  u32 duration = (u32)ktime_to_us(ktime_get() - last);

  // ⚠️注意：1838红外头高低电平逻辑取反
  if (!gpio_get_value(ir.gpio)) {
    ir.space = duration;
  } else {
    // 切换下降沿时，脉冲只有高电平部分，所以不做处理
    ir.pulse = duration;
    goto irq_out;
  }

  if (is_head(ir.pulse, ir.space)) {
    ir.count = ir.data = 0;
  } else if (is_repeat(ir.pulse, ir.space)) {
    ir.count = 32;
  } else if (is_btrue(ir.pulse, ir.space)) {
    ir.data |= 1 << ir.count++;
  } else if (is_bfalse(ir.pulse, ir.space)) {
    ir.data |= 0 << ir.count++;
  } else {
    goto irq_out;
  }

  if (ir.count >= 32) {
    wake_up(&ir.rwait);
  }

irq_out:
  mod_timer(&ir.timer, jiffies + (HZ / 10));
  last = ktime_get();
  return IRQ_HANDLED;
}

// 定时清除红外协议帧的相关信息，便于接收下一帧
static void clear_flag(struct timer_list *timer) {
  ir.pulse = 0;
  ir.space = 0;
  ir.count = 0;
  ir.data = 0;
}

static ssize_t ir_read(struct file *filp, char __user *buf, size_t len, loff_t *off) {
  int rc = 0;

  if ((filp->f_flags & O_NONBLOCK) && ir.count < 32) {
    return -EAGAIN;
  } else {
    DECLARE_WAITQUEUE(wq, current);
    add_wait_queue(&ir.rwait, &wq);
    wait_event(ir.rwait, ir.count == 32);
    remove_wait_queue(&ir.rwait, &wq);
  }

  rc = copy_to_user(buf, &ir.data, sizeof(u32));
  if (rc < 0) {
    return rc;
  }

  ir.count = 0;
  *off += sizeof(u32);
  return sizeof(u32);
}

static const struct file_operations fops = {
  .owner = THIS_MODULE,
  .read = ir_read,
};

static struct miscdevice irdev = {
  .minor = MISC_DYNAMIC_MINOR,
  .name = "IR1838-NEC",
  .fops = &fops,
  .nodename = "ir0",
  .mode = 0744,
};

static int __init ir_init(void) {
  int rc = 0;

  // 初始化脉冲处理函数
  init_waitqueue_head(&ir.rwait);

  // 初始化定时器，用于断帧
  timer_setup(&ir.timer, clear_flag, 0);
  add_timer(&ir.timer);

  // 申请GPIO及其双边沿中断
  ir.gpio = 18;
  if ((rc = gpio_request_one(ir.gpio, GPIOF_IN, "IR")) < 0) {
    printk(KERN_ERR "ERROR%d: can not request gpio%d\n", rc, ir.gpio);
    return rc;
  }

  ir.irq = gpio_to_irq(ir.gpio);
  if ((rc = request_irq(ir.irq, ir_rx,
              IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
              "IR", NULL)) < 0) {
    printk(KERN_ERR "ERROR%d: can not request irq\n", ir.irq);
    return rc;
  }

  if ((rc = misc_register(&irdev)) < 0) {
    return rc;
  }

  return 0;
}
module_init(ir_init);

static void __exit ir_exit(void) {
  misc_deregister(&irdev);
  free_irq(ir.irq, NULL);
  gpio_free(ir.gpio);
  del_timer(&ir.timer);
}
module_exit(ir_exit);
```

以下是应用层的测试代码，有关CAR-MP3遥控器的指令码网上一搜一大把，如果你不嫌烦，也可以一个一个的试出来。

由于驱动层是直接把原始数据的<地址><地址取反><指令><指令取反>高低位反转后，直接给到进程，所以进程read出来的数据，指令码应该在第3段(16-24位)。

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fcntl.h>

// car-mp3遥控器指令码
static const char* keyname[] = {
  [0x45] = "Channel-",  [0x46] = "Channel", [0x47] = "Channel+",
  [0x44] = "Speed-",    [0x40] = "Speed+",  [0x43] = "Play/Pause",
  [0x15] = "Vol+",      [0x07] = "Vol-",    [0x09] = "EQ",
  [0x16] = "No.0",      [0x19] = "100+",    [0x0d] = "200+",
  [0x0c] = "No.1",      [0x18] = "No.2",    [0x5e] = "No.3",
  [0x08] = "No.4",      [0x1c] = "No.5",    [0x5a] = "No.6",
  [0x42] = "No.7",      [0x52] = "No.8",    [0x4a] = "No.9",
};

int main(int argc, char* argv[]) {
  int ir = open("/dev/ir0", O_RDONLY);

  while (1) {
    int frame = 0;
    if (read(ir, &frame, sizeof(int)) < 0) {
      perror("read ir");
      break;
    }

    int cmd = (frame >> 16) & 0xFF;
    printf("%s\n", keyname[cmd]);
  }

  close(ir);
  return 0;
}
```

## 小结

- NEC协议采用PWM编码，一个完整的脉冲周期表示一个bit
- 1838红外接收头状态取反
- 别看我写的这么轻松，前几天刚接触红外时简直被搞疯😫