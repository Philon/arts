package account

import "fmt"

// Administrator 管理员用户
// 通过类型嵌套，继承User
type Administrator struct {
	User
	Level int
}

// Administrator实现show接口
func (a Administrator) show() {
	fmt.Printf("Administrator %s(%d): Level-%d\n", a.Name, a.ID, a.Level)
}
