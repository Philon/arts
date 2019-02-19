package main

import (
	"./account"
)

func main() {
	// 定义并初始化一个结构
	user := account.User{
		ID:   112233,
		Name: "philon",
	}

	// 定义一个空结构
	var admin account.Administrator
	admin.Name = "root"
	admin.Level = 123

	// 由于嵌套继承，admin也可以调用user定义好的方法
	user.SetPassword("123")
	admin.SetPassword("456")

	// admin和user各自实现了show接口，ShowInfo属于多态
	account.ShowInfo(user)  // User philon<112233>
	account.ShowInfo(admin) // Administrator root(0): Level-123

	account.Register(user)
	// account.Register(admin) 👈 Register不是接口，不能传入Administrator类型
	user.Unregister()
	if _, exists := account.GetUser("philon"); !exists {
		println("removed user successfully")
	}
}
