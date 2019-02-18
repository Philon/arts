# 《GO语言实战》学习笔记五：类型系统

类型——比“类”多了一个字而已，如果懂面向对象的话，类型的很多内容其实和面向对象**语言**如出一辙，但不同于面向对象**思想**。换而言之，Java/C#等常见的类的属性、方法、接口、继承等等形式，在GO的类型系统里都能找到身影，但其实现过程和思路却截然不同。

为了巩固本章的知识点，我仿照传统的MVC架构来实现一个“账户管理”的业务，作为GO与Java在实现面向对象方面的思想类比。在源码的`account`包中包含：
- `user`——用户类型，基础模型
- `admin`——管理员类型，“继承”于user
- `show`——视图，用户信息打印，用接口实现user和admin多态
- `manage`——服务，负责用户的增删改查

## 类型的定义

在GO语言中定义一个类是非常简单的：`type user struct {}`，当然，它不叫类，而是结构类型，很像C语言中的结构体。

在`user.go`里声明了两个类型：`Password`和`User`，高手一看便知Password其实就是内置的string类型，感觉两者是可以互换的。然而一旦做了这样的声明之后，GO的编译器就会吧Password和string严格当作两种独立的类型来处理。换句话说，不能把string定义的变量直接赋值给Password类型的对象，只能在初始化时接收纯字符串。

```go
type Password string

type User struct {
	ID       int      // 包外可见
	Name     string   // 包外可见
	password Password // 包内可见
}
```

此外，需要注意GO语言的符号可见性。以下情况适用于函数、变量、方法、属性等：
- 首字母大写的符号——包外可见
- 首字母小写的符号——仅包内可见

结构类型的使用也非常简单：
```go
// 定义并初始化一个结构
user := User{
	ID:   112233,
	Name: "philon",
}
// 或者
user := User{112233, "philon", "123456"}

// 修改属性
user.password = "56789"
```

## 类型的方法

根据面向对象的套路，定义了类型的属性，自然少不了要定义方法，但GO语言不允许直接把一个类型的方法定义在其内部，而是可以将其定义在任何其他地方。如果习惯了Java这种方式，可能会觉得十分别扭，但这正是GO类型灵活的地方。如果你觉得某个“业务对象”需要某个特殊的方法来处理，那就直接加上好了，不影响它的继承、耦合等问题。

给类型添加一个方法的基本语法为：
```go
// u表示接收者，可以理解为面向对象中的一个对象
func (u User) method_name() {
	u.name = "philon"
	u.email = "xxx@xxx.com"
}
```

不过添加方法时需要注意“接受者”的区别，主要有两种：
- `值接收者`的方法在其内部修改对象的值，不改变外部调用者
- `指针接收者`的方法在其内部修改对象的值，改变外部调用者

还是以代码为例：

```go
// 值接受者方法
func (u User) Auth(p Password) bool {
	u.password = "123456" // 👈此行并不影响外部调用对象
	return u.password == p
}

// 指针接受者方法
func (u *User) SetPassword(p Password) {
	u.password = p // 👈同时修改了外部调用对象的属性
}
```

如果理解函数调用的内存管理，那这两种形式非常容易理解。调用函数的时候，传入的参数将拷贝一个副本并压栈，函数通过访问栈区来获取参数值。换句话说：**所有传入函数的参数其实都只是副本**，在函数内部修改副本的值，不会影响原始参数值。

✍️**但务必注意**✍️

Go语言里的引用类型：切片、映射、通道、接口和函数是比较特殊的，前几章已经说明了它们作为参数在函数间传递时，本身就是以引用形式传递，所以**引用类型的方法，值接受者其实是个引用(指针)副本**。千万小心。

## 嵌入类型(继承)

`User`类型的属性和方法都实现了，在面向对象里面自然少不了继承，例如管理员账户`Administrator`类型一般而言都会继承User。GO语言对继承的形式如下：

```go
// Administrator 管理员用户
type Administrator struct {
	User  // 通过嵌套，继承“父类”
	Level int
}

// 使用方式
a := Administrator{
	User: { 1, "root", "password" },
	Level: 123
}
// 或者
var a Administrator
a.ID = 1
a.Name = "root"
a.Level = 123
```

## 接口

接口在第二章中其实总结了很多了，GO语言的接口属于`鸭子类型`——也就是一个类型只要实现了接口的方法，不管它们是否存在继承关系，都能够以多态的形式调用。

这里以`Shower`接口为例，该接口要求实现一个`show`方法，用于打印一个类型的内部信息。和面向对象的思路一样，各个类型实现自己的show方法，而Shower只负责调用。

首先是接口的定义：
```go
// Shower 显示接口的定义
// 根据GO的规则，如果一个接口只有一个方法，那就叫方法名+er
type Shower interface {
	show()
}

// ShowInfo 任何实现了show的类型都可以传入该函数
func ShowInfo(s Shower) {
	s.show()
}
```

下面是`User`和`Administrator`的不同show实现：
```go
// User类型实现show接口
func (u User) show() {
	fmt.Printf("User %s<%d>\n", u.Name, u.ID)
}

// Administrator实现show接口
func (a Administrator) show() {
	fmt.Printf("Administrator %s(%d): Level-%d\n", a.Name, a.ID, a.Level)
}

// 假设我们定义了两个类型的变量，并通过ShowInfo调用
ShowInfo(user)  // User philon<112233>
ShowInfo(admin) // Administrator root(0): Level-123
```

从上边的例子可以看到，user和admin虽然是不同的类型，但是由于都实现了show这个接口方法，因此都可以传入`ShowInfo`函数，并且也实际调用到了它们各自的实现。

## 小结一下
- type xxx struct 定义一个结构类型
- type xxx interface 定义一个接口
- 不管是类型、函数还是成员变量，首字母大写表示包外可见，否则包内可见
- type xxx struct { OtherType }即可继承其它结构类型
- func (t Type) foo() 定义一个类型的方法
- 值接受者定义的类型方法，调用者传入副本，方法内修改对象不影响外部
- 指针接受者定义的类型方法，调用者传入地址，方法内修改对象影响外部
- 接口是实现多态的类型，只要实现了接口方法，任何类型都可以调用
- 标识符首字母大写是包外公开，小写仅包内公开