package pool

import (
	"errors"
	"io"
	"log"
	"sync"
)

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
