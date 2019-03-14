package main

import (
	"fmt"
	"strconv"
	"testing"
)

// BenchmarkFormat 测试fmt.Sprintf将数字转换为字符串的效率
func BenchmarkSprintf(b *testing.B) {
	number := 10

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_ = fmt.Sprintf("%d", number)
	}
}

// BenchmarkFormat 测试strconv.FormatInt将数字转化为字符串的效率
func BenchmarkFormat(b *testing.B) {
	number := int64(0)

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		strconv.FormatInt(number, 10)
	}
}

// BenchmarkItoa 测试strconv.Itoa将数字转换为字符串的效率
func BenchmarkItoa(b *testing.B) {
	number := 10

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		strconv.Itoa(number)
	}
}
