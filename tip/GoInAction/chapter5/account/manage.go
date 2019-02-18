package account

import "fmt"

// 因为首字母小写，变量包外不可见，相当于protected
var users = make(map[string]User)
var maxID = 0

// Register 添加一个新用户并返回用户对象
func Register(u User) {
	if _, exists := users[u.Name]; exists {
		fmt.Printf("%s already exists\n", u.Name)
	} else {
		u.ID = maxID
		users[u.Name] = u
		maxID++
	}
}

// Unregister User的一个方法，删除用户
// 根据在业务需要的地方，灵活增加类型的方法
func (u User) Unregister() {
	if _, exists := users[u.Name]; exists {
		delete(users, u.Name)
	} else {
		fmt.Printf("%s does not exists\n", u.Name)
	}
}

// GetUser 从列表中获取用户对象
func GetUser(name string) (User, bool) {
	user, exists := users[name]
	return user, exists
}
