package main

import (
	"io"
	"log"
	"math/rand"
	"sync"
	"sync/atomic"
	"time"

	"./pool"
)

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
