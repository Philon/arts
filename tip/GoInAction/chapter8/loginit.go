package main

import "log"

/**

引用自 golang.org/src/log/log.go

// 这些标示是用于定义Logger每次输出时候的前缀。
// Bits are or'ed together to control what's printed.
// There is no control over the order they appear (the order listed
// here) or the format they present (as described in the comments).
// The prefix is followed by a colon only when Llongfile or Lshortfile
// is specified.
// For example, flags Ldate | Ltime (or LstdFlags) produce,
//	2009/01/23 01:23:23 message
// while flags Ldate | Ltime | Lmicroseconds | Llongfile produce,
//	2009/01/23 01:23:23.123123 /a/b/c/d.go:23: message
const (
	Ldate         = 1 << iota     // 日期: 2009/01/23
	Ltime                         // 时间: 01:23:23
	Lmicroseconds                 // 毫秒级时间: 01:23:23.123123.  覆盖 Ltime.
	Llongfile                     // 完整的源码文件路径及行号: /a/b/c/d.go:23
	Lshortfile                    // 短路径及行号: d.go:23. 会覆盖 Llongfile
	LUTC                          // 如果设置了Ldata或Ltime，采用UTC取代本地时区
	LstdFlags     = Ldate | Ltime // 标准日志初始值
)
*/

// 初始化后，所有直接调用log包的日志输出都会受影响
func init() {
	// 设置日志的前缀信息
	log.SetPrefix("[From log package] ")
	// 设置日志的中段标示，参考golang.org/src/log/log.go
	log.SetFlags(log.Ldate | log.Lmicroseconds | log.Lshortfile)
}
