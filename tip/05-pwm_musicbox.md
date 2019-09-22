# 树莓派驱动开发实战05：PWM音乐盒

本文源码：https://github.com/Philon/rpi-drivers/tree/master/05-pwm_musicbox

上一篇用LED呼吸灯的方式，基本介绍了PWM原理以及在树莓派上的驱动开发，但总感觉意犹未尽，所以再写一篇PWM的应用场景——PWM+蜂鸣器，实现一个简易的音乐盒。

说明一下： 本篇纯粹是“玩”，并不涉及任何新的知识点，如果不感兴趣可以掠过。

先来看看我实现的效果：《保卫黄河》、《灌篮高手》、《欢乐斗地主》。这些谱子全是我从网上扒下来的，并根据蜂鸣器的效果修改过，自己不是专业搞音乐的，所以难免会有错误的地方。(反正我耳朵里听着没问题就行😁)

```sh
philon@rpi:~ $ cd modules/
philon@rpi:~/modules $ sudo insmod musicbox.ko
philon@rpi:~/modules $ ./player_test music/01-保卫黄河      # 🎵
philon@rpi:~/modules $ ./player_test music/04-灌篮高手主题曲 # 🎵
philon@rpi:~/modules $ ./player_test music/05-欢乐斗地主    # 🎵
```

<iframe src="//player.bilibili.com/player.html?aid=68617894&cid=118922238&page=1" scrolling="no" border="0" frameborder="no" framespacing="0" allowfullscreen="true" style="width:960px;height:640px"> </iframe>

再来看看我是怎么实现的：

1. 实现PWM蜂鸣器驱动`musicbox`：通过`write`音符给设备节点，播放不同的声音；通过`ioctl`控制节拍
2. 实现应用层`player_test`，负责读取歌曲的乐谱
3. 编写乐谱，其实就是文本简谱，类似下面这首

```
C 3/4

# ~前奏~
(5` (4`) 3` 2` 1` 7 1` 0 3 (2) 3 5 (6 5 6 1`) 5 0) 
(6` (5`) 4` 3` 2` 1` 7 6 5 6 1` 2` 5 6 2` 3` 5 6 3` 4` 5 6 4` 5`) 

# 风在吼，马在叫，黄河在咆哮，黄河在咆哮
1` (1` 3) 5- 1` (1` 3) 5-  (3) 3 (5) 1` 1` (6) 6 (4) 2` 2` 

# 河西山岗万丈高，河东河北高粱熟了
(5 (6) 5 4) (3 2 3 0) (5 (6) 5 4) (3 2 3 1)

# 万山丛中，抗日英雄真不少
5 (6) 1` 3 (5 (3`) 2` 1`) 5 6 3- 

# 青纱帐里，游击健儿真不少
5 (6) 1` 3 (5 (3`) 2` 1`) 5 6 1`- 

# 端起了土枪洋枪，挥动着大刀长矛
(5 (3 5) 6 5 1` 1`) 0 (5 (3 5) 6 5 2` 2`) 0 

# 保卫家乡，保卫黄河，保卫华北，保卫全中国
(5 (6) 1` 1`) 0 (5 (6) 2` 2`) 0 (5 (6) 3` 3`) (5 (6) 3` 2` 1`----)
```

好，具体实现且听我慢慢道来～

## PWM蜂鸣器驱动

有关蜂鸣器硬件原理、有源、无源这里不展开讨论。总之本文采用的是树莓派上的PWM0+一个无源蜂鸣器。接线如下图所示：

![PWM蜂鸣器树莓派接线图](https://i.loli.net/2019/09/22/K1znqZhc6Bx4G3u.png)
![PWM蜂鸣器原理图](https://i.loli.net/2019/09/22/VqfoJl5IXT8uMwm.png)

根据上一篇《PWM呼吸灯》的学习，基本知道PWM对脉冲的控制主要有`占空比`和`脉冲周期`两部分。用来控制LED的时，占空比可以调节灯光的强弱，在脉冲周期似乎没什么乱用。

对于蜂鸣器声用作声乐，有三个基本要素：音调、节拍、音量大小。
- 音调：由震动频率决定，对应PWM的脉冲周期
- 音量：同样的频率，PWM占空比越高，声音越大
- 节拍，声音的持续时长，和PWM毛关系都没有，做个定时开关即可

综上，其实在蜂鸣器驱动`musicbox`里重点实现两个接口：
- `write`: 解析用户层写入的字符串，例如音调do的高中低音分别为'`` 1` ``'和'`` 1 ``'和'`` 1. ``'，然后换算出对应的`频率`即可。
- `ioctl`: 解析用户层发来的指令，有节拍、音调、音量等控制。

### 不同音调的蜂鸣器频率

注意：此部分涉及的乐理知识我不是很懂，基本是从网上抄来的，但我发现F和B调的发音不是很准，估计频率不对。

下表分别是`Do Re Mi Fa So La Ti`对应的蜂鸣器震动频率。

wait-！7个音符，怎么会干出13种频率呢？

因为其中涵盖了A-G不同曲调，一首曲子可以由多个调子来演奏，比如我们经常听到的C小调，D大调之类的。其中的乐理只是更为复杂，这里只需要记住：

以`C调的Do`为基准，其他调子做相应偏移。例如E调的Do相当于C调的Mi，而A调的Do相当于C调的La。

| 音域 |  1  |  2  |  3  |  4  |  5  |  6  | C7  | D7  | E7  | F7  | G7  | A7  | B7  |
|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
| 低音 | 131 | 147 | 165 | 175 | 196 | 221 | 248 | 278 | 312 | 330 | 371 | 416 | 467 |
| 中音 | 262 | 294 | 330 | 350 | 393 | 441 | 495 | 556 | 624 | 661 | 742 | 883 | 935 |
| 高音 | 525 | 589 | 661 | 700 | 786 | 882 | 900 | 1112| 1248| 1322| 1484| 1665| 1869|

### 如何计算PWM的周期

有了不同音符的震动频率，也就得到了PWM的脉冲周期。举个例子，50Hz相当于1秒钟震动50次，那PWM的脉冲周期就应该为1s/50=0.02秒。因此周期的计算公式为：

period = 1s / freq

其中的freq就是音符表中的频率，而1s可以由Linux中的`HZ`变量表示。

### 如何计算PWM占空比

有了脉冲周期，才能计算占空比。一个周期内高电平所占时间越大，输出声音也就越大。所以我们可以通过百分比来决定占空比大小。

假设现在要输出高音`` 3` ``，它对应的频率为661，并根据前面的公式求得脉冲周期为12345，而音量为75%，那占空比应该为12345*75/100 = 9528。因此占空比的计算公式为：

duty = period * volume / 100

### 如何计算节拍

所谓节拍，如2/4拍，表示以4分音符为一拍，每小节有两拍。

但在程序里，节拍即每个音符输出的时长，这一点我并没有在驱动层实现，但做的做法非常简单。并没有引入“小节”和“动次打次”的概念。就是强制一个小节为4秒，如果是2/4拍，就相当于4000/4/2 = 500毫秒，即每个音符默认响0.5秒。如果存在半拍的情况(就是音符下有画线)，那时间再减半。

程序中把半拍用圆括号`()`表示，遇到左括号就减半时间，遇到右括号就加倍时间，就是那么粗暴。

### 驱动程序实现

当加载以下驱动后，可以通过命令行`echo 1 > /dev/musicbox`来测试是否会响。

```c
#include <linux/module.h>
#include <linux/pwm.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/timer.h>
#include <linux/wait.h>

#define MUSICBOX_MAX_VOLUME 100 // 最大音量100%
#define ONE_SECOND  1000000000  // 以纳秒为单位的一秒

// 各音域音符对应震动频率
static const int tones[][14] = {
  //   C    D     E     F     G     A     B
  //   1.   2.    3.    4.    5.    6.    7.
  {0, 131, 147,  165,  175,  196,  221,  248,  278,  312,  330,  371,  416,  476},
  //   1    2     3     4     5     6     7 
  {0, 262, 294,  330,  350,  393,  441,  496,  556,  624,  661,  742,  833,  935},
  //   1'   2'    3'    4'    5'    6'    7'
  {0, 525, 589,  661,  700,  786,  882,  990,  1112, 1248, 1322, 1484, 1665, 1869},
};

static struct {
  bool                playing;    // 是否正在播放
  wait_queue_head_t   wwait;      // 写等待
  struct pwm_device*  buzzer;     // 蜂鸣器
  struct timer_list   timer;      // 定时器
  char                volume;     // 音量 0-100
  char                tonality;   // 音调 A-G
  char                beat;       // 节拍
  char                key;        // 音调
} musicbox;

static void music_stop(struct timer_list* timer) {
  pwm_disable(musicbox.buzzer);
  musicbox.playing = false;
  wake_up(&musicbox.wwait);
}

static ssize_t musicbox_write(struct file *filp, const char __user *buf, size_t len, loff_t *off) {
  // 获取音符
  int note = (buf[0] - '0') + musicbox.key;
  // 获取音域
  int pitch = (buf[1] == '`') ? 2 : (buf[1] == '.' ? 0 : 1);
  // 根据频率计算脉冲周期
  int tone = ONE_SECOND / tones[pitch][note];
  // 根据脉冲周期计算音量
  int volume = tone * musicbox.volume / 100;
  // 当音符后跟着'-'就延长一倍时间
  int delay = HZ / musicbox.beat * (len - (pitch != 1));

  // 写阻塞，一次只能播放一个音符
  if (musicbox.playing) {
    if (filp->f_flags & O_NONBLOCK) {
      return -EAGAIN;
    } else {
      DECLARE_WAITQUEUE(wq, current);
      add_wait_queue(&musicbox.wwait, &wq);
      wait_event(musicbox.wwait, !musicbox.playing);
      remove_wait_queue(&musicbox.wwait, &wq);
    }
  }

  pwm_config(musicbox.buzzer, volume, tone);
  if (buf[0] > '0') {
    pwm_enable(musicbox.buzzer);
  }
  mod_timer(&musicbox.timer, jiffies + delay);
  musicbox.playing = true;

  return len;
}

// 音量、音调、节拍控制
static long musicbox_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {
  switch (cmd) {
  case MUSICBOX_SET_VOLUMN:
    if (arg >= 0 && arg <= MUSICBOX_MAX_VOLUME) {
      musicbox.volume = arg;
    } else {
      return -EINVAL;
    }
    break;
  case MUSICBOX_GET_VOLUMN:
    return musicbox.volume;
  case MUSICBOX_SET_BEAT:
    if (arg > 0 && arg <= 1000) {
      musicbox.beat = 1000 / arg;
    } else {
      return -EINVAL;
    }
    break;
  case MUSICBOX_GET_BEAT:
    return 1000 / musicbox.beat;
  case MUSICBOX_SET_KEY:
    if (arg < 'A' || arg > 'G') {
      return -EINVAL;
    }
    musicbox.key = arg >= 'C' ? (arg - 'C') : (arg - 'A' + 5);
    break;
  case MUSICBOX_GET_KEY:
    return musicbox.key;
  default:
    printk("error cmd = %d\n", cmd);
    return -EFAULT;
  }
  return 0;
}

// 以下是设备驱动注册/注销相关
static const struct file_operations fops = {
  .owner = THIS_MODULE,
  .write = musicbox_write,
  .unlocked_ioctl = musicbox_ioctl,
};

static struct miscdevice mdev = {
  .minor = MISC_DYNAMIC_MINOR,
  .name = "musicbox",
  .fops = &fops,
  .nodename = "musicbox",
  .mode = S_IRWXUGO,
};

int __init musicbox_init(void) {
  musicbox.buzzer = pwm_request(0, "Buzzer");
  if (IS_ERR_OR_NULL(musicbox.buzzer)) {
    printk(KERN_ERR "failed to request pwm0\n");
    return PTR_ERR(musicbox.buzzer);
  }

  musicbox.volume = 50;
  musicbox.tonality = 'A';
  musicbox.key = 0;
  musicbox.beat = 4;

  init_waitqueue_head(&musicbox.wwait);
  timer_setup(&musicbox.timer, music_stop, 0);
  add_timer(&musicbox.timer);

  misc_register(&mdev);

  return 0;
}
module_init(musicbox_init);

void __exit musicbox_exit(void) {
  misc_deregister(&mdev);
  del_timer(&musicbox.timer);
  pwm_disable(musicbox.buzzer);
  pwm_free(musicbox.buzzer);
}
module_exit(musicbox_exit);
```

## 应用层加载乐谱

应用层`player_test`程序的业务逻辑就简单得多了：
1. 加载指定的乐谱文件
2. 配置音乐盒的节拍、音调
3. 按行读取文件内容(跳过注释行和空行)
4. 提取每一行的音符、括号
5. 将音符、节拍写入驱动
6. 重复第3-5步，直至文件末尾

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>

#include "musicbox.h"

#define MUSIC_BOX_FILE  "/dev/musicbox"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Usage: ./player <musicfile>");
    return -1;
  }

  int fd = open(MUSIC_BOX_FILE, O_RDWR);
  if (fd < 0) {
    perror("open musicbox");
    exit(0);
  }

  FILE* music = fopen(argv[1], "r");
  if (music == NULL) {
    perror("open music");
    exit(0);
  }

  // 初始化音乐盒
  char line[128] = {'\0'};
  if (fgets(line, sizeof(line), music) == NULL) {
    perror("read music");
    exit(0);
  }

  // 4秒为一节，计算每拍的长度，如2/4拍时，每拍长度为500ms
  int beat = 4000 / (line[4]-'0') / (line[2]-'0');
  if (ioctl(fd, MUSICBOX_SET_BEAT, beat) < 0
   || ioctl(fd, MUSICBOX_SET_VOLUMN, 90) < 0
   || ioctl(fd, MUSICBOX_SET_KEY, line[0]) < 0) {
    perror("ioctl");
    exit(0);
  }

  // 按行加载乐谱文件
  while (fgets(line, sizeof(line), music)) {
    printf("%s", line);
    if (line[0] == '#' || line[0] == '\0') {
      continue;
    }

    char* p = line;
    while (*p) {
      if (*p == '(') {
        ioctl(fd, MUSICBOX_SET_BEAT, ioctl(fd, MUSICBOX_GET_BEAT) / 2);
      } else if (*p == ')') {
        ioctl(fd, MUSICBOX_SET_BEAT, ioctl(fd, MUSICBOX_GET_BEAT) * 2);
      } else if (*p >= '0' && *p <= '7') {
        char* q = p+1;
        while (*q == '`') q++;
        while (*q == '.') q++;
        while (*q == '-') q++;
        write(fd, p, q-p);
      }
      p++;
    }
  }

  close(fd);
  fclose(music);
  return 0;
}
```

## 小结

由于本章没有新的知识点，就不做知识总结了，说说感受。

当蜂鸣器按照我的预期演奏音乐是还是挺开心的，仿佛一下子把我拉回了大学的那个暑假，一个人默默在宿舍鼓弄51单片机的日子。时光荏苒，尽管做的是同一件事，但我现在的软件架构、编程基础不可同日而语。或许我重拾底层技术的同时，也重拾了当年学习的热情吧😊。