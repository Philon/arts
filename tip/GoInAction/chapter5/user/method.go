package user

import "fmt"

// user 自定义一个用户类型
type user struct {
	name  string
	email string
}

// notify 一个user值接收者的方法
func (u user) notify() {
	fmt.Printf("Sending User Email to %s<%s>\n", u.name, u.email)
	u.name = "Philon" // 👈这里虽然修改了name属性，但由于u是值(副本)，调用者本身的name属性不会变化
}

// changeEmail 一个user指针接收者的方法
func (u *user) changeEmail(email string) {
	u.email = email // 👈由于u是指针接收者，相当于修改外部调用者的email属性
}

// Function 函数及类型接收者的两种不同调用方式示例
func Function() {
	//
	u1 := user{"philon", "inexsist@philon.cn"}
	u2 := user{
		name:  "张三",
		email: "隔壁老王@zhangsan.home",
	}

	fmt.Println("===========值接收者调用===========")
	u1.notify()
	u2.notify()
	fmt.Printf("u1.name = %s, u2.name = %s\n", u1.name, u2.name)
	fmt.Println("***********函数内的修改对外部无效***********")

	fmt.Println()
	fmt.Println("===========指针接收者调用===========")
	u1.changeEmail("u1@philon.cn")
	u2.changeEmail("u2@philon.cn")
	fmt.Printf("u1.email = <%s>, u2.email = <%s>\n", u1.email, u2.email)
	fmt.Println("***********函数内的修改对外部有效***********")
}
