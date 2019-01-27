package search

import (
	"log"
	"sync"
)

// 创建一个匹配器映射，存储不同类型的匹配器
var matchers = make(map[string]Matcher)

// Register 外部模块调用来注册自己的匹配器，其实就是给rss.go和default.go用
func Register(feedType string, matcher Matcher) {
	if _, exists := matchers[feedType]; exists {
		log.Fatalln(feedType, "Matcher already registered !")
		return
	}

	log.Println("Register", feedType, "matcher")
	matchers[feedType] = matcher
}

// Run 执行关键字新闻搜索流程
func Run(keyword string) {
	// 加载用户订阅表
	feeds, err := RetrieveFeeds()
	if err != nil {
		log.Print(err)
		return
	}

	// 创建一个无缓冲的通道，用于存储每个并发任务的返回结果
	results := make(chan *Result)

	// 创建主监听器，用于监听所有并发任务
	var waitGroup sync.WaitGroup
	waitGroup.Add(len(feeds))
	// 启动goroutine，等待所有任务结束，最后关闭通道
	go func() {
		waitGroup.Wait()
		close(results)
	}()

	// 根据订阅表，启动相应数量的并发任务，执行新闻抓取和关键字匹配
	for _, feed := range feeds {
		// 根据订阅源的类型获取对应的搜索匹配器(其实这里只有rss类型)
		matcher, exists := matchers[feed.Type]
		if !exists {
			matcher = matchers["default"]
		}

		// 启动goroutine，执行新闻搜索
		go func(matcher Matcher, feed *Feed) {
			Match(matcher, feed, keyword, results)
			waitGroup.Done()
		}(matcher, feed)
	}

	// 显示相关信息
	Display(results)
}
