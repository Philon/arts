package account

import "fmt"

// Password 账户密码
// 尽管表面上等同于string，但GO会认为这个两个独立的类型
type Password string

// User 定义一个类型：用户账户
// 因为改类型命名首字母大些，所以包外可见，相当于public
// 结构类型的成员同样遵循公开/私有的命名方式
type User struct {
	ID       int      // 包外可见
	Name     string   // 包外可见
	password Password // 包内可见
}

// Auth 密码验证
// 这是一个值接受者方法
func (u User) Auth(p Password) bool {
	return u.password == p
}

// SetPassword 修改用户密码
// 这是一个指针接受者方法
func (u *User) SetPassword(p Password) {
	u.password = p
	fmt.Printf("%s password set to %s\n", u.Name, u.password)
}

// User类型实现show接口
func (u User) show() {
	fmt.Printf("User %s<%d>\n", u.Name, u.ID)
}
