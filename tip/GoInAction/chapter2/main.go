package main

import (
	"log"
	"os"

	_ "./matchers"
	"./search"
)

func init() {
	// 将log的默认输出设置为系统的标准输出
	log.SetOutput(os.Stdout)
}

func main() {
	// 搜索含有boy的相关新闻
	search.Run("boy")
}
