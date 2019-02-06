package main

import "fmt"

// PointArray 指针数组
func PointArray() {
	// 常规数组复制
	// 直接通过赋值表达式，可以复制数组内容
	color1 := [...]string{"Red", "Green", "Blue"}

	var color2 [3]string
	color2 = color1
	color1[0] = "Yellow"

	fmt.Printf("color1: %v\n", color1) // color1: [Yellow Green Blue]
	fmt.Printf("color2: %v\n", color2) // color2: [Red Green Blue]

	// 指针数组复制
	// 由于数组内的元素是指针地址，所以不论如何复制
	// 改变其中一个地址内容，也相当于改变了另一个
	nums1 := [...]*int{new(int), new(int), new(int)}
	*nums1[0] = 10
	*nums1[1] = 20
	*nums1[2] = 30

	var nums2 [3]*int
	nums2 = nums1
	*nums2[2] = 99 // 相当于修改nums1[2]

	fmt.Print("nums1: [")
	for _, v := range nums1 {
		fmt.Printf("%d ", *v)
	}
	fmt.Println("]") // nums1: [10 20 99 ]

	fmt.Print("nums2: [")
	for _, v := range nums2 {
		fmt.Printf("%d ", *v)
	}
	fmt.Println("]") // nums2: [10 20 99 ]
}

func main() {

	ArrayCreate()
	ArrayAccess()
	ArrayClone()
	SliceCreate()
	SliceClone()
}
