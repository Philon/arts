package account

// Shower 显示接口的定义
// 根据GO的规则，如果一个接口只有一个方法，那就叫方法名+er
type Shower interface {
	show()
}

// ShowInfo 任何实现了show的类型都可以传入该函数
// 多态
func ShowInfo(s Shower) {
	s.show()
}
