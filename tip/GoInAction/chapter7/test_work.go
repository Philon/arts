package main

import (
	"log"
	"sync"
	"time"

	"./work"
)

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
