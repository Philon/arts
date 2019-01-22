package search

import (
	"encoding/json"
	"os"
)

// Feed 用户的web订阅表映射，参考data/data.json
type Feed struct {
	Name string `json:"npr"`
	URI  string `json:"link"`
	Type string `json:"type"`
}

// 这里采用相对路径，相对于程序的执行路径(不是相对feed.go)
const dataFile = "data/data.json"

// RetrieveFeeds 加载data/data.json，并反序列化到Feed切片(指针)中
func RetrieveFeeds() ([]*Feed, error) {
	// 打开dataFile
	file, err := os.Open(dataFile)
	if err != nil {
		return nil, err
	}

	// 如果打开成功，defer可以在函数返回时，执行关闭文件
	defer file.Close()

	// 定义一个Feed的切片指针，并将文件中的数据反序列化存储到切片中
	var feeds []*Feed
	err = json.NewDecoder(file).Decode(&feeds)

	return feeds, err
}
