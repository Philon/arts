package main

import "fmt"

// MapCreate 映射的创建和初始化
func MapCreate() {
	// 创建一个空的映射
	a := make(map[string]int)
	// 创建一个映射，并初始化两个元素
	b := map[string]string{"Red": "#da1337", "Orange": "#e95a22"}

	// 直接插入一个新元素
	a["hello"] = 5

	// 使用delete删除一个元素
	delete(b, "Red")

	// 根据键索引，可获取对象值、是否存在两个值
	value, exists := b["Blue"]
	if !exists {
		b["Blue"] = "#1e90ff"
	} else {
		println(value)
	}

	fmt.Println("========MapCreate========")
	fmt.Printf("a(%d): %v\n", len(a), a) // map[hello:5]
	fmt.Printf("b(%d): %v\n", len(b), b) // map[Blue:#1e90ff Orange:#e95a22]

	colors := map[string]string{
		"AliceBlue": "#f0f8ff",
		"Coral":     "#ff7F50",
		"DarkGray":  "#a9a9a9",
	}

	// 通过for-range访问map
	for key, value := range colors {
		fmt.Printf("colors[%s] = %s\n", key, value)
	}
}
