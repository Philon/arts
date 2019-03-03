package main

import (
	"log"
	"os"
	"time"

	"./runner"
)

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
