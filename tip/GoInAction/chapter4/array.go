package main

import "fmt"

// ArrayCreate 数组创建例子
func ArrayCreate() {
	// 创建并初始化一个长度为3的数组
	a1 := [3]int{1, 2, 3}
	// 创建并初始化一个数组，长度根据初始化元素决定
	a2 := [...]int{1, 2, 3, 4, 5}
	// 声明一个长度为5的数组，元素为0值
	var a3 [5]int
	// 创建一个长度为5的数组，初始化个别元素
	a4 := [5]int{1: 3, 4: 55}

	fmt.Println("========ArrayCreate========")
	fmt.Printf("a1(%d): %v\n", len(a1), a1)
	fmt.Printf("a2(%d): %v\n", len(a2), a2)
	fmt.Printf("a3(%d): %v\n", len(a3), a3)
	fmt.Printf("a4(%d): %v\n", len(a4), a4)
}

// ArrayAccess 数组访问
func ArrayAccess() {
	a := [...]int{1, 2, 3, 4, 5}

	a[1] = 22
	a[3] = 44

	fmt.Println("========ArrayAccess========")
	for i, v := range a {
		fmt.Printf("a[%d] = %d\n", i, v)
	}
}

// ArrayClone 数组克隆例子
func ArrayClone() {
	a := [...]int{1, 2, 3, 4, 5}
	// 完全复制
	b := a
	// 从a[1]开始，复制长度为3-1=2个
	c := a[1:3]

	fmt.Println("========ArrayAccess========")
	fmt.Printf("a(%d): %v\n", len(a), a)
	fmt.Printf("b(%d): %v\n", len(b), b)
	fmt.Printf("c(%d): %v\n", len(c), c)
}
