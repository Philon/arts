# 《GO语言实战》学习笔记五：并发

但凡复杂一点的业务，并发基本跑不了，其实说白了无非是多线程/多进程架构，可一旦涉及并发模式，少不了调度、同步、互斥、资源访问等一堆堆问题，解决这些问题又需要一堆堆代码，这些代码不仅维护难度高，而且可以说和业务本身没有半毛钱关系(纯粹的技术问题有木有)。

But！GO语言对并发的运用相比其它语言还是相当愉快的。根据本章内容可以学到，GO语言的“多线程”机制goroutine，以及多线程之间的通信方式channel。

## GO运行时(runtime)

在说GO的并发之前需要先搞清楚一件事，否则后面会一头雾水。

由于前几章并没有特别说明，加上`go build`命令可以直接生成一个独立的可执行文件(而且没有任何依赖)，会让人误以为go程序是类似c/c++一样的机器码。其实不然，**GO的可执行文件内部嵌入了runtime，本质上和Java/.Net一样跑在虚拟机之上**。

GO的运行时负责内存管理、垃圾回收、栈处理等等，而其中一个很重要的功能便是goroutine和channel的管理。

通常情况下，GO运行时默认给整个应用程序分配一个逻辑处理器，逻辑处理器会绑定到物理处理器上。一个GO程序默认最多创建10000个线程，但可以通过runtime包的`SetMaxThreads`方法来修改，程序里的线程超出最大线程数会导致程序崩溃。

## goroutine

所以通过上述要搞懂，goroutine不是系统分配的线程，更不归操作系统调度，一切都是靠运行时分配和调度。但不论如何，为了便于前期的学习和理解并发，这里默认goroutine就是线程。

先来创建10个线程：
```go
func main() {
	var wg sync.WaitGroup
	wg.Add(10) // WaitGroup计数加10

	for i := 0; i < 10; i++ {
		// 创建goroutine
		go func(id int) {
			fmt.Printf("Goroutine-%d\n", id)
			wg.Done() // WaitGroup计数减1
		}(i)
	}

	wg.Wait() // 等待，直到WaitGroup计数为0
}
```

代码输出：
```
Goroutine-2
Goroutine-9
Goroutine-1
Goroutine-3
Goroutine-5
Goroutine-8
Goroutine-4
Goroutine-7
Goroutine-6
Goroutine-0
```

从上面代码可以看出，for循环只是顺序创建了10个goroutine，但输出是并行的，没有顺序。

再强调一遍，**goroutine不是线程**

尽管从效果上看，goroutine就是线程，但事实是上边的程序只有一个线程，交由go运行时维护，线程内部会自动负责多个goroutine的调度管理。

## 并发竞争

有并发的地方，就有资源竞争。只需要把上边的代码稍作改动：
```go
var count int // 声明一个全局变量

func main() {
	var wg sync.WaitGroup
	wg.Add(5)

	for i := 0; i < 5; i++ {
		go func() {
			defer wg.Done() // 👈匿名函数退出时调用

			var num = count // 读全局变量
			fmt.Printf("num = %d\n", num)
			count = num + 1 // 写全局变量
		}()
	}

	wg.Wait()
	// 所有goroutine结束后，count值并不为5
	fmt.Printf("count = %d\n", count)
}
// ----------程序输出结果(每次都是随机的)--------
num = 0
num = 1
num = 0
num = 0
num = 0
count = 1
```

上述这段代码可以非常明显的看到，5个goroutine都对全局变量`count`做了加1的运算，结果count最终值却是1而非5。这也说明，在没有措施的情况下并发去访问全局变量会出现诡异的结果。

道理很简单，一个goroutine再访问某个资源时另一个goroutine可能正在写，导致访问结果不符合预期，或者你前脚刚写，后脚就被他人覆盖了。要解决这个问题，GO语言提供了两种传统思路：
1. 原子操作函数，确保每次访问都是完整的读写
```go
// atomic包里还有很多如读取、写入等安全访问函数
// 这里仅使用加法计算
atomic.AddInt64(&count, 1)
```

2. 互斥锁，我在访问的时候你不准访问
```go
var mutex sync.Mutex // 用来定义代码临界区
mutex.Lock() // 加锁，其它goroutine会被阻塞
...
mutex.Unlock() // 解锁，其它goroutine继续运行
```

## 通道

原子函数和互斥锁都可以很好地解决资源共享的问题，但它们都不够优秀，因为你不得不考虑程序的运行逻辑、优先级之类的问题。仔细想想，其实我们访问共享资源无非是为了生产/消费数据，只是为了确保数据能被安全访问才引入这样那样的竞争机制。那有没有一种办法能让开发者专注处理数据，不要去操心那些毫不相关的业务逻辑。答案就是GO的通道机制。

简单来说，一个goroutine需要读数据的时候，就从通道里去拿，处理完了就放回通道，至于那些资源互斥等问题，运行时已经处理得很完美了。

### 通道的基本使用

- 用`make`和`close`来创建和关闭通道
- 通道一般运行在goroutine函数内
- 使用`<-`完成通道数据的读写

```go
var wg sync.WaitGroup

// 接收端goroutine
func receive(data chan int) {
	num := <-data // 将通道数据读取到变量
	fmt.Printf("received number: %d\n", num)
	wg.Done()
}

// 发送端goroutine
func send(data chan int, num int) {
	fmt.Printf("sent number: %d\n", num)
	data <- num // 将数据写入通道
	wg.Done()
}

// UseChannel 通道的基本使用
func UseChannel() {
	channel := make(chan int) // 创建一个通道
	wg.Add(2)
	go receive(channel)
	go send(channel, 20)
	wg.Wait()
	close(channel) // 关闭通道
}
```

如上代码所示，通过`channel := make(chan int)`创建了一个int类型的通道，且通过该通道实现在`recevie`和`send`两个goroutine之间的数据通信，注意`channel <- value`表示写通道，`value <- channel`表示读通道。

另外，GO提供两种通道机制，无缓冲通道和有缓冲通道。

### 无缓冲通道

顾名思义，无缓冲就是在通道内不没有缓冲空间，对于两个goroutine而言，需要双方同时做好准备才能进行数据传递，否则先做好准备的一方就会阻塞，等待另一方做好准备。如下图所示：

![无缓冲通道示意图](https://i.loli.net/2019/02/24/5c7239ab48e43.png)

其实最开始关于`send`和`recevie`的例子就是典型的无缓冲通道，所以具体的用法就不再赘述了。

留意一下两个函数中`fmt.Printf`的顺序，发送者是在发送数据之前打印，而接受者是在接收数据之后打印。不过，两个函数是goroutine，理论上来说独自运行，打印没有先后次序，但上边的例子不论运行多少次都是先打印`”sent number: xx"`再打印`”received number: xx"`。由此可见，因为没有缓冲，`num := <-data`的时候，如果data通道的对面没有在写入，这里就会被阻塞。

### 有缓冲通道

同理，有缓冲就是在通道内有缓冲空间，对于两个goroutine而言，无所谓对方有没有做好准备，它们只需要关系通道内的缓存有没有数据，如下图所示：

![有缓冲通道示意图](https://i.loli.net/2019/02/24/5c7239d3d806f.png)

下面这段代码也展示了如果运用有缓冲通道，其实非常简单，就是在创建通道的时候指定一下通道的缓存长度`make(chan <type>, <length>)`即可，其它地方几乎不用变。
```go
func BufferChannel() {
	// 创建一个长度为10的有缓冲通道
	bufferedChannel := make(chan int, 10)
	wg.Add(10)

	// 启动5个goroutine接收数字
	for i := 0; i < 5; i++ {
		go receive(bufferedChannel)
	}
	// 启动5个goroutine发送随机5个数字
	for i := 0; i < 5; i++ {
		go send(bufferedChannel, rand.Intn(100))
	}

	wg.Wait()
	close(bufferedChannel)
}

//---------------程序输出结果------------
sent number: 81
received number: 81
sent number: 81
sent number: 87
sent number: 47
received number: 81
received number: 87
received number: 47
sent number: 59
received number: 59
```

从这样的程序输出结果可以明显看到，`receive`和`send`的执行根本互不影响，不存在阻塞的情况，否则就不会出现连续发送和连续接收的打印了。

但是需要注意一点，发送数字和接收数字的顺序确实一样的，也就是说有缓冲通道内部，数据是按照先进先出的方式在管理。

## 小结一下

- GO语言并发是指goroutine，由GO的运行时负责管理
- 使用`go`关键字来创建goroutine
- `sync/atomic`和`sync.Mutex`可以解决并发时的资源竞争问题
- 相比于原子函数和互斥锁，GO语言的通道机制可以更好地处理共享数据
- 使用`make(chan <type>)`创建无缓冲通道
- 使用`make(chan <type>, <length>)`创建有缓冲通道