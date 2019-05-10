# 嵌入式Linux驱动学习六：并发和竞态

临界资源：是指同一个时段内只允许唯一一个访问者操作的资源。比如打印机、IO模块等，但Linux是多任务的，其内核对资源的管理是抢占式的。多个进程同时运行即所谓的并发，而如果多个进程都同时访问同一个资源就会产生竞态。由于驱动模块的特殊性，它不可避免会存在被多个进程同时“打开、读写、关闭”的情况。设想一下，如果某个驱动的逻辑是open的时候分配一块缓存用于read/write，close的时候又释放缓存，就会存在A进程刚打开的设备节点，B进程就关闭，缓存分配了又释放，最终在读写时导致程序崩溃。

所以，本章主要学习Linux驱动模块有哪些手段可以处理并发时的竞态问题。

## 原子操作

原子操作就是保证对数据修改的完整性，也就是说`a = a + 1`这么简单的表达式也难以避免被编译为多个指令周期，也许在任务A中刚读完表达式右值，又被任务B更新了`a`的寄存器，结果一个简单的自加1的操作都可能出现很多诡异的结果。

因此，为了确保`i++`就是自加1的操作，内核封装了很多API以实现变量的原子操作：
```c
#include <asm/atomic.h> # 引入原子操作API

// 定义原子变量，并将其初始化为0
atomic_t v = ATOMIC_INIT(0);

// 变量的原子读写操作
int atomic_read(atomic_t* v);
void atomic_set(atomic_t* v, int i);

// 变量运算的原子操作
void atomic_add(int i, atomic_t* v);    // 加
void atomic_sub(int i, atomic_t* v);    // 减
int atomic_and(int i, atomic_t* v);     // 与
int atomic_or(int i, atomic_t* v);      // 或
int atomic_xor(int i, atomic_t* v);     // 异或
int atomic_andnot(int i, atomic_t* v);  // 与非

// 在运算的基础之上“返回原来的结果”
int atomic_fetch_add(int i, atomic_t* v);
int atomic_fetch_sub(int i, atomic_t* v);
int atomic_fetch_and(int i, atomic_t* v);
int atomic_fetch_or(int i, atomic_t* v);
int atomic_fetch_xor(int i, atomic_t* v);
int atomic_fetch_andnot(int i, atomic_t* v);

void atomic_add_return(int i, atomic_t* v); // 加，并返回新值
void atomic_sub_return(int i, atomic_t* v); // 减，并返回新值

/******************************************
 * 强烈注意：
 *  以下定义在ARM平台(或Linux5.0+)不存在
 *  尽管各大书籍和网络文章里依然这么介绍
******************************************/
void atomic_int(atomic_t* v); // 自增
void atomic_dec(atomic_t* v); // 自减

// 带测试的加减运算，如果操作后原子值为0，则返回true，反之false。
int atomic_inc_and_test(atomic_t* v);
int atomic_dec_and_test(atomic_t* v);
int atomic_sub_and_test(int i, atomic_t* v);
// 注意不是atomic_add_and_test
int atomic_add_negative(int i, atomic_t* v);
```

## 自旋锁

自旋锁是一种对临界资源互斥访问的手段，也就是说在访问资源之前上个锁，访问完成后解锁，如果一个进程在访问资源是发现“锁住”了，就会原地打转——而非进入睡眠！直到锁被解开。就好比一辆车遇到红灯后停了下来但没熄火，发动机一直在空转，直到绿灯。但自旋锁有个很大的弊端——“如果红绿灯刚好坏了，发动机会永远空转下去”。

先来看看自旋锁的简单用法：
```c
#include <linux/spinlock.h> // 引入自旋锁的头

// 定义并初始化一个锁
spinlock_t lock;
spin_lock_init(&lock);

// 获取锁状态，有两种方式
spin_lock(&lock);     // 如果锁住了,原地打转
spin_trylock(&lock);  // 如果锁住了,立即返回,不会锁死

// todo...各种临界资源访问和处理

spin_unlock(&lock);   // 解锁，为后来的访问者开绿灯
```

上边是最简单的使用方式，但自旋锁还会受到内核中断、底半部(BH)的影响，所以衍生出了更多的“锁定”和“解锁”API。就好比驾驶员在等红灯时跑去尿尿，恰好此时绿灯亮起，该怎么办？答：禁止驾驶员尿尿😄。

这些函数要视情况具体使用：
```c
void spin_lock_irq(spinlock_t* lock);   // 禁用中断，并上锁
void spin_unlock_irq(spinlock_t* lock); // 启用中断，并解锁

// 同上，但保存/恢复状态字
void spin_lock_irqsave(spinlock_t* lock, unsigned long flags);
void spin_unlock_irqrestore(spinlock_t* lock, unsigned long flags);

void spin_lock_bh(spinlock_t* lock);    // 禁用bh，并上锁
void spin_unlock_bh(spinlock_t* lock);  // 启用bh，并解锁
```

开发驱动时应谨慎使用自旋锁，要直到它“空转”的意思是不放弃CPU，所以在其自旋时会对CPU资源造成浪费，如果不小心锁死了，那就悲催了。

综上，**自旋锁只是在访问临界资源前后加了一层排他性的锁**，至于锁内的资源操作它完全不关心，然而共享资源在并发访问时往往是这样的需求：**可以被同时读，但不允许同时写**。也是基于此，内核提供了更多的API来满足这些场景。

1. **读写自旋锁**

读写自旋锁会区别读和写的资源，满足并发读取，单一写入的要求，但底层也是“自旋”的机制。
```c
// 读写锁定义
rwlock_t lock;
rwlock_init(&lock);

// 读取上锁/解锁
read_lock(&lock);
// todo...
read_unlock(&lock);

// 写入上锁/解锁
write_lock(&lock);
// todo...
write_unlock(&lock);
```

2. **顺序锁**

顺序锁是读写锁的优化版，因为读写锁的读和写操作是互斥的，所以使用顺序锁后，当资源正在写入时，依然可以被读取。

```c
// 顺序锁API的定义
unsigned int read_seqbegin(const seqlock_t* sl);
unsigned int read_seqretry(const seqlock_t *sl, unsigned int start);
void write_seqlock(seqlock_t *sl);
void write_sequnlock(seqlock_t *sl);

/*-----------------以下是具体使用方法-----------------*/
#include <linux/seqlock.h>

// 顺序锁定义
seqlock_t lock;
seqlock_init(&lock);

// 顺序读的过程
unsigned int start = 0;
do {
  start = read_seqbegin(&lock);
  // todo...read
} while (read_seqretry(&lock, start));

// 写入上锁
write_seqlock(&lock);
// todo...write
write_sequnlock(&lock);
```

3. **RCU**: Read-Copy-Update

读——复制——更新的意思是：把要写的部分先读取被拷贝一个副本，然后把内容写入副本，等到何时的时机一把更新到源。

```c
#include <linux/rcupdate.h>

void rcu_read_lock(void);
void rcu_read_unlock(void);


void synchronize_rcu(void);
void call_rcu(struct callback_head *head, rcu_callback_t func);
```

## 信号量与互斥体

## 完成量