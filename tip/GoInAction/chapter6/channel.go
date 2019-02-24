package main

import (
	"fmt"
	"math/rand"
	"sync"
)

var wg sync.WaitGroup

func receive(data chan int) {
	num := <-data // 将通道数据读取到变量
	fmt.Printf("received number: %d\n", num)
	wg.Done()
}

func send(data chan int, num int) {
	fmt.Printf("sent number: %d\n", num)
	data <- num // 将数据写入通道
	wg.Done()
}

// UnbufferChannel 无缓冲通道的基本使用
func UnbufferChannel() {
	unbufferedChannel := make(chan int) // 创建一个无缓冲通道
	wg.Add(2)
	go receive(unbufferedChannel)
	go send(unbufferedChannel, 20)
	wg.Wait()
	close(unbufferedChannel) // 关闭通道
}

// BufferChannel 有缓冲通道的基本使用
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
