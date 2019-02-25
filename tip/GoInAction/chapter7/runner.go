package main

import (
	"errors"
	"os"
	"os/signal"
	"sync"
	"time"
)

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
