package search

import (
	"fmt"
	"log"
)

// Result 保存新闻抓取的结果数据
type Result struct {
	Field   string
	Content string
}

// Matcher 匹配器接口，定义了新闻搜索的行为
type Matcher interface {
	Search(feed *Feed, keyword string) ([]*Result, error)
}

// Match 执行新闻搜索、关键字匹配
func Match(matcher Matcher, feed *Feed, keyword string, results chan<- *Result) {
	searchResults, err := matcher.Search(feed, keyword)
	if err != nil {
		log.Println(err)
		return
	}

	// 将搜索-匹配后的结果写入通道
	for _, result := range searchResults {
		results <- result
	}
}

// Display 显示(打印)搜索结果
func Display(results chan *Result) {
	for result := range results {
		fmt.Printf("%s: \n%s\n\n", result.Field, result.Content)
	}
}
