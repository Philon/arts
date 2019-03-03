# 《GO语言实战》学习笔记七：并发模式

我们可以通过goroutine和channel机制非常方便地编写并发业务，但就和面向对象与设计模式的关系一样，是一种思想具体落实到行动方针的过程，在牛逼的战略，没有基本的战术指导，也只是空谈。

因此，第七章并发模式，并没有太多语法上的新东西，而是利用goroutine和channel介绍了三种并发模式，分别适用于三种不同的业务场景。
1. runner——给每个并发任务设置deadline，管理并发任务的生命周期
2. pool——利用有缓冲通道创建资源池，统一管理并发时的资源访问
3. work——利用无缓冲通道创建goroutine池，统一管理并发

## runner

先假设一个场景需求，比如http服务的并发，我们要为每个来自客户端的请求创建一个临时的并发响应任务，但这个最好在某个规定的时间内完成响应，否则就强制它退出，这样可以很好地避免某些情况下，一些并发任务卡死的情况，同时可以很好地管理每个并发的生命周期。  

runner就是为这样的场景应用而生的，runner可以理解为是一个运行管理器，所有的并发任务都要叫给它负责管理，它负责并发任务的启动、超时监控、强制中断等。

(由于我个人在阅读原著的时候是先讲runner的内部实现，再看实际应用，总感觉云里雾里的，觉得还是先通篇看一下如何运用runner，再来看其内部的实现，可能效果会好一点)

先来看runner的示例：

```go
func testRunner() {
	log.Println("Runner test starting work...")

	// 新建一个runner，并强制每个并发任务的超时时间为5秒
	r := runner.New(3 * time.Second)
	// 循环创建10个并发任务，并将其丢给runner管理
	for i := 0; i < 5; i++ {
		r.Add(func(id int) {
			// 这里只是模拟，每个并发任务都是睡眠它自身id的秒数
			log.Printf("Processor - Task #%d.\n", id)
			time.Sleep(time.Duration(id) * time.Second)
			log.Printf("Task #%d done.\n", id)
		})
	}

	// 一次性启动runner内部的全部并发任务
	if err := r.Start(); err != nil {
		switch err {
		case runner.ErrTimeout:
			// 当并发任务中有任务执行超时，就立即返回
			log.Println("Terminating due to timeout.")
			os.Exit(1)
		case runner.ErrInterrupt:
			// 当程序被ctrl+c时，强制结束所有并发任务
			log.Println("Terminating due to interrupt.")
			os.Exit(2)
		}
	}

	log.Println("Process end.")
}

//-------------程序输出---------------
2019/03/03 14:48:41 Runner test starting work...
2019/03/03 14:48:41 Processor - Task #2.
2019/03/03 14:48:41 Processor - Task #4.
2019/03/03 14:48:41 Processor - Task #3.
2019/03/03 14:48:41 Processor - Task #0.
2019/03/03 14:48:41 Task #0 done.
2019/03/03 14:48:41 Processor - Task #1.
2019/03/03 14:48:42 Task #1 done.
2019/03/03 14:48:43 Task #2 done.
// ----第3个及以后的任务因为要睡3秒以上，肯定会超时----
2019/03/03 14:48:44 Terminating due to timeout.
// ----如何运行过程中按ctrl+c，会安全退出并提示----
^C2019/03/03 14:49:36 Terminating due to interrupt.
```

可以看到，runner就是一个类型，需要用其创建对象后才能具体使用。而在外部，我们只需要定义好每个任务的函数，并简单的将它们添加到runner当中即可，剩下的全部交由runner自行管理。

现在再来看看runner类型是如何实现的：
```go
// Runner 在指定的超时时间内完成一组任务
// 并且在这个时间周期内接收系统的中断信号来结束这组任务
type Runner struct {
	// 从系统接收中断信号的通道
	interrupt chan os.Signal
	// 任务已完成的报告通道
	complete chan error
	// 任务超时的报告通道
	timeout <-chan time.Time
	// 任务列表
	tasks []func(id int)
}

// ErrTimeout 任务执行超时时返回
var ErrTimeout = errors.New("received timeout")

// ErrInterrupt 收到系统中断信号时返回
var ErrInterrupt = errors.New("received interrupt")

// New 创建Runner的工厂函数
func New(d time.Duration) *Runner {
	return &Runner{
		interrupt: make(chan os.Signal, 1),
		complete:  make(chan error),
		timeout:   time.After(d),
	}
}

// Add Runner的方法，将多个任务添加到Runner的任务列表中
func (r *Runner) Add(tasks ...func(int)) {
	r.tasks = append(r.tasks, tasks...)
}

// Start Runner的方法，启动所有任务，并监听通道事件
func (r *Runner) Start() error {
	// 开始接收系统的中断通知
	signal.Notify(r.interrupt, os.Interrupt)

	// 通过gorouting并行启动所有任务列表中的任务
	var wg sync.WaitGroup
	wg.Add(len(r.tasks))
	for i, t := range r.tasks {
		go func(id int, task func(int)) {
			task(id)
			wg.Done()
		}(i, t)
	}
	// 等待所有任务执行完成，并给“已完成通道”一个报告
	go func() {
		wg.Wait()
		r.complete <- nil
	}()

	// 获取无缓冲通道数据时，如果没准备好，会被阻塞
	select {
	case err := <-r.complete: // 任务正常实行完返回任务自身的“错误标示”
		return err
	case <-r.timeout: // 任务执行超时，返回超时错误
		return ErrTimeout
	case <-r.interrupt: // 如果收到ctrl+C则停止接收后续的信号
		signal.Stop(r.interrupt)
		return ErrInterrupt
	}
}
```

由于只是原型演示，runner的内部实现不算复杂，只需要记住一个核心思想**无缓冲通道在没有数据读写的时候，会被阻塞**。说千道万，runner就是利用了这个特性才得以在select语句中完成了：
- 并行接收系统的中断信号——interrupt通道。
- 并行接收定时器的超时信号——timeout通道。

## pool

这里的pool是指资源池的意思，如果熟悉Java/C#中的“数据连接池”的概念，那这里的池大体就是这个意思了。

换而言之，在并发场景下，难免会遇到并发任务争夺临界资源的情况，还是以数据库访问为例：如果有1000个并发任务要去访问数据库，每个并发都需要完成建立连接——认证——查询——断开连接等操作，那不论是应用服务器还是数据库服务器，无疑都是巨大的负担。因此，通过创建10个数据库连接，并把这些“连接”当作资源放入“池”中，给所有的并发任务共享，每个并发在需要的时候从池中取出连接，完成查询后再放回池中，不仅能大幅降低CPU的负载，也能减少内存的开销。(但我个人觉得最爽的地方是，你的代码可以更专注地去query，而不必考虑connection本身😂)

同样，先来看看pool的运用过程：
```go
const (
	maxGoroutines  = 25 // 要使用的goroutine的数量
	pooledResource = 5  // 池中的资源的数量
)

// dbConnection 模拟要共享的资源
type dbConnection struct {
	ID int32
}

// Close 实现io.Closer.Close接口，释放资源
// 让其可以被Pool管理
func (dbConn *dbConnection) Close() error {
	log.Println("Close: Connection", dbConn.ID)
	return nil
}

// idCounter 用来给每个连接分配唯一id
var idCounter int32

// createConnection 创建唯一id的连接
func createConnection() (io.Closer, error) {
	id := atomic.AddInt32(&idCounter, 1)
	log.Println("Create: New Connection", id)

	return &dbConnection{id}, nil
}

// testPool Pool测试用例
func testPool() {
	var wg sync.WaitGroup
	wg.Add(maxGoroutines)

	// 创建管理连接池，并创建N个“连接”资源，加入池中
	p, err := pool.New(createConnection, pooledResource)
	if err != nil {
		log.Println(err)
	}

	// 创建M个并发任务，模拟查询数据库
	for query := 0; query < maxGoroutines; query++ {
		go func(q int) {
			performQueries(q, p)
			wg.Done()
		}(query)
	}

	wg.Wait()
	log.Println("Shutdown Program")
	p.Close() // 关闭池
}

func performQueries(query int, p *pool.Pool) {
	conn, err := p.Acquire()
	if err != nil {
		log.Println(err)
		return
	}

	// 完成查询后，将资源释放会池里
	defer p.Release(conn)

	// 用随机睡眠1000微妙内的时长，来模拟查询中的耗时
	time.Sleep(time.Duration(rand.Intn(1000)) * time.Millisecond)
	log.Printf("QID[%d] CID[%d]\n", query, conn.(*dbConnection).ID)
}
```

(可能是我没学习到位，我个人觉得pool模式并不是特别容易掌握，思想是很好理解的，但牵扯太多接口实现、有/无缓冲通道的特性等内容，所以代码可能要再多消化几遍。)

再看看pool包的实现：
```go
// Pool 资源池
// 管理一组资源，可以安全地在多个goroutine共享
// 实现 io.Closer接口
type Pool struct {
	m        sync.Mutex
	resource chan io.Closer
	factory  func() (io.Closer, error)
	closed   bool
}

// ErrPoolClosed 资源池已关闭的错误标示
var ErrPoolClosed = errors.New("Pool has been closed")

// New 创建Pool的工厂函数
func New(fn func() (io.Closer, error), size uint) (*Pool, error) {
	if size <= 0 {
		return nil, errors.New("size value too small")
	}

	return &Pool{
		factory:  fn,
		resource: make(chan io.Closer, size),
	}, nil
}

// Acquire 从资源池中获取一个资源
func (p *Pool) Acquire() (io.Closer, error) {
	select {
	case r, ok := <-p.resource:
		// 检查是否有空闲资源
		log.Println("Acquire:", "Shared Resource")
		if !ok {
			return nil, ErrPoolClosed
		}
		return r, nil
	default:
		// 如果没有可用资源，就创建一个
		log.Println("Acquire:", "New Resource")
		return p.factory()
	}
}

// Release 释放一个资源，将其放回资源池
func (p *Pool) Release(r io.Closer) {
	p.m.Lock()
	defer p.m.Unlock()

	// 如果该资源已经被关闭，销毁这个资源
	if p.closed {
		r.Close()
		return
	}

	select {
	case p.resource <- r:
		// 试图将该资源加入队列
		log.Println("Release:", "In Queue")
	default:
		// 如果队列已满，关闭这个资源
		log.Println("Release:", "Closing")
		r.Close()
	}
}

// Close 关闭资源池中的所有资源，并停止工作
func (p *Pool) Close() {
	p.m.Lock()
	defer p.m.Unlock()

	if p.closed {
		return
	}

	p.closed = true

	close(p.resource)

	// 关闭所有资源
	for r := range p.resource {
		r.Close()
	}
}
```

pool资源池实现的核心思想是**有缓冲通道读写时不会引起阻塞，select语句在通道内没有数据的情况下会自动执行default选项**。


## work

work模式就是创建一个goroutine池，管理池中所有的goroutine统一执行。但它有别于runner模式，runner其实是负责监控池中的每个并发任务的生命周期的，而work则是负责池中的每个并发任务的执行顺序，即任务队列。

这个模式的好处在于，可以很好地控制程序运行的负载，比如突发情况下，某台服务器的http请求一瞬间到达100万，如果为了响应所有请求也在一瞬起启动100万个响应任务，那估计服务器就冒烟了。所以最好的方式就是限制并发任务数量，比如每次最多启动1万个响应，剩下的排队慢慢来。

因此，work就是一个并发任务的队列池，还是先看看如何运用的：
```go
var names = []string{
	"steve",
	"bob",
	"mary",
	"therese",
	"jason",
}

type namePrinter struct {
	name string
}

// Task 实现Worker接口
func (m *namePrinter) Task() {
	log.Println(m.name)
	time.Sleep(time.Second)
}

func testWork() {
	// 设置工作池的“工位”为2，即每次只能有两个人工作
	p := work.New(2)

	var wg sync.WaitGroup
	wg.Add(100 * len(names)) // 每人肩负100项任务，共5人

	// 一次性把5 * 100个任务全部丢到工作池中
	// 相当于创建了500个goroutine
	for i := 0; i < 100; i++ {
		for _, name := range names {
			np := namePrinter{name: name}
			go func() {
				p.Run(&np) // 将对象的任务丢到工作池中统一管理执行
				wg.Done()
			}()
		}
	}

	// 等待所有任务在工作池中被完成
	wg.Wait()
	p.Shutdown()
}
```

如上代码，namePrinter实现了work包内规定的`Task`接口之后，work的工作池就能够统一管理namePrinter对象了。这个namePrinter可以理解为某个业务的模拟，比如上面说的http响应任务(这里仅是简单地做个打印)。

而后，不论创建多少个namePrinter相关的goroutine(并发)，都只需简单地将其丢到工作池中Run(p.Run并没有立刻启动任务，工作池会根据情况自行安排)。

最后在看看work包的实现：
```go
// Worker 必须满足接口，才能使用工作池
type Worker interface {
	Task()
}

// Pool 工作池，相当于goroutines池管理
type Pool struct {
	work chan Worker
	wg   sync.WaitGroup
}

// New 创建工作池的工厂函数
func New(maxGoroutines int) *Pool {
	p := Pool{
		work: make(chan Worker),
	}

	p.wg.Add(maxGoroutines)
	for i := 0; i < maxGoroutines; i++ {
		go func() {
			// p.work是通道，所有创建goroutine之后
			// for循环会被阻塞，直到p.work被关闭为止
			for w := range p.work {
				w.Task()
			}
			p.wg.Done()
		}()
	}

	return &p
}

// Run 提交工作到工作池
func (p *Pool) Run(w Worker) {
	p.work <- w
}

// Shutdown 等待所有goroutines结束
func (p *Pool) Shutdown() {
	close(p.work)
	p.wg.Wait()
}
```

work的实现其实是非常简单的，**核心思想是for-range循环时，无缓冲通道会阻塞**，工作池是一个无缓冲通道，而每个for-range都相当于一个队列，当池中有数据是，所有的for-range都会争夺这个输入数据来处理，但如果某个队列本身已经在工作时，就没空再争夺通道内的数据。可以说是最简单有效的负载均衡。

## 小结一下
- 无缓冲通道在读写时会引起阻塞，可以用来控制程序生命周期
- 带default分支的select语句会尝试读写通道，而不会阻塞
- 可以利用无缓冲通道创建一个工作池，统一管理goroutine并发任务
- 可以利用有缓冲通道创建一个资源池，统一管理并发时的资源访问