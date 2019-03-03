package work

import "sync"

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
