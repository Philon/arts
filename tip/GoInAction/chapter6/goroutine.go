package main

import (
	"fmt"
	"sync"
	"sync/atomic"
)

// CreateGoroutines 创建并发的基本方式
func CreateGoroutines() {
	var wg sync.WaitGroup
	wg.Add(10) // WaitGroup计数加10

	for i := 0; i < 10; i++ {
		// 创建goroutine
		go func(id int) {
			fmt.Printf("Goroutine-%d\n", id)
			wg.Done() // WaitGroup计数减1
		}(i)
	}

	// 直到WaitGroup的计数为0，否则一直阻塞
	wg.Wait()
}

// AccessResource 无锁机制下并发访问资源，导致诡异结果
func AccessResource() {
	var wg sync.WaitGroup
	wg.Add(10)
	var count int // 声明一个“全局”变量，所有goroutine都可以访问

	for i := 0; i < 10; i++ {
		go func() {
			defer wg.Done()               // 👈匿名函数退出时调用
			var num = count               // 读全局变量
			fmt.Printf("num = %d\n", num) // 每个goroutine打印的num可能是一样的值
			count = num + 1               // 写全局变量
		}()
	}

	// 所有goroutine结束后，count值并不为10(0-10随机)
	wg.Wait()
	fmt.Printf("count = %d\n", count)
}

// AccessResourceByAtomic 通过原子函数访问全局变量
func AccessResourceByAtomic() {
	var wg sync.WaitGroup
	wg.Add(10)
	var count int32 // 声明一个“全局”变量，所有goroutine都可以访问

	for i := 0; i < 10; i++ {
		go func() {
			defer wg.Done()
			atomic.AddInt32(&count, 1) // 通过原子操作，count+1
			fmt.Printf("num = %d\n", count)
		}()
	}

	// 所有goroutine结束后，count值为10
	wg.Wait()
	fmt.Printf("count = %d\n", count)
}

// AccessResourceByMutex 通过互斥访问全局变量
func AccessResourceByMutex() {
	var wg sync.WaitGroup
	wg.Add(10)
	var mutex sync.Mutex // 用来定义代码临界区
	var count int32      // 声明一个“全局”变量，所有goroutine都可以访问

	for i := 0; i < 10; i++ {
		go func() {
			defer wg.Done()
			mutex.Lock() // 访问加锁
			var num = count
			fmt.Printf("num = %d\n", count)
			count = num + 1
			mutex.Unlock() // 访问解锁
		}()
	}

	// 所有goroutine结束后，count值为10
	wg.Wait()
	fmt.Printf("count = %d\n", count)
}
