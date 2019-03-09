# 《GO语言实战》学习笔记八：标准库
什么是GO语言的标准库？就是放在`$GOROOT//usr/local/go/pkg`目录下的那些文件，它们由GO语言社区共同维护的，经过良好设计，确保代码稳定且易用。每次发布GO的新版本时，都会将这些库打包成`.a`静态库文件。

标准库中有非常多的基本功能，不用在为业务开发而重新造轮子，比如我们最熟悉的fmt，以及log、json、http、网络、图像处理、加密算法等等。

本章仅对log、json、io三个包的调用方式及基本原理做总结，乍一看可能会觉得本章只是带着你了解一下几个函数的基本用法，别不以为然，我个人的理解，本章最最最精华的内容就是最后那句话——**阅读标准库的代码时熟悉GO语言习惯的好方法**。

所以，没事多看看官方文档：[http://golang.org/pkg/](http://golang.org/pkg/)

## 定制自己的Logger

日志是每个程序开发最常用的功能了，一般来说C/C++/Java/C#都会有第三方的日志框架实现，用起来都挺顺手。不过，GO语言标准库中已经包含了日志框架——log包，不用再满世界地去比较到底哪个框架好用了。

先来看看log包的基本用法：

```go
/**
引用自 golang.org/src/log/log.go
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
  log.SetPrefix("[LogPrefix] ")
  // 设置日志的中段标示，参考上边的注释
  log.SetFlags(log.Ldate | log.Lmicroseconds | log.Lshortfile)
}
```

上边的代码很简单(前提是忽略掉注释部分)，在init函数中先初始化log的基本格式，就是在输出到屏幕时的样式。

调用更简单：
```go
package main

import "log"

func main() {
  log.Println("hello world")
}

// ------------以下是程序输出--------------
[LogPrefix] 2019/03/09 14:34:06.763601 main.go:6: hello world
```

init函数中，log被设置了前缀`[LogPrefix] `以及中间部分的完整时间日志+文件名及行号，所以一个简单的helloworld日志信息前会自动追加很多有效调试信息。

这里只需要记住一点，log包一旦被设置，全局有效！

那么问题来了，如果我需要两种以上不同的日志格式怎么办？答——**log.Logger**。

先定义一套自己的Logger规则：
```go
var (
  Trace   *log.Logger // 普通跟踪调试信息
  Info    *log.Logger // 特殊信息
  Warning *log.Logger // 警告日志
  Error   *log.Logger // 错误日志(输出到文件)
)
```

如上，定义了4个logger对象，分别用于：
- 普通信息：程序正式发布时，该部分不再输出
- 特殊信息：总是打印到屏幕上
- 警告：类似于“特殊信息”，你要愿意，也可以修改其字体颜色
- 错误：同时将日志打印到屏幕，保存至文件

有了这4套机制的需求后，再来看看它们是如何被实现的：
```go
func init() {
  file, err := os.OpenFile("error.log", os.O_CREATE|os.O_WRONLY|os.O_APPEND, 0666)
  if err != nil {
    log.Fatalln("Failed to open error file: ", err)
  }

// 因为ioutil.Discard，所有通过Trace打印的日志都不会输出
    Trace = log.New(ioutil.Discard, "TRACE: ", log.Ldate|log.Ltime|log.Lshortfile)
    Info = log.New(os.Stdout, "INFO: ", log.Ldate|log.Ltime|log.Lshortfile)
    Warning = log.New(os.Stdout, "WARNING: ", log.Ldate|log.Ltime|log.Lshortfile)
    // io.MultiWriter表示多种输出渠道，即同时打印到屏幕和文件
    Error = log.New(io.MultiWriter(file, os.Stderr), "ERROR: ", log.Ldate|log.Ltime|log.Llongfile)
}
```

核心只有一个：`log.New(out io.Writer, prefix string, flag int)`函数，会根据参数设置好logger的输出目的地、前缀信息、日志标示等，并将其返回。

留意一下，`Trace`这个logger的输出对象是`ioutil.Discard`，其实就是不输出的意思；而另一处logger的对象Error关于`io.MultiWriter()`表示多目标输出，可以看到，既要输出到file，还要输出到stderr。

好了，至于这4个logger的调用方式嘛，和标准库中的log是一模一样的。

## 从Github上获取用户信息

为了学习调用GO的标准库进行json编解码，书中是用Google的API请求来举例……所以我觉得还是用GitHub来演示，效果会好一点。~~anyway~~ anywhere，只要明白json序列化和反序列化即可。

### 反序列化

先用浏览器随便GET个用户信息试试，比如就我自己的地址：https://api.github.com/users/philon
```json
{
  "login": "Philon",
  "id": 2968783,
  "node_id": "MDQ6VXNlcjI5Njg3ODM=",
  "avatar_url": "https://avatars0.githubusercontent.com/u/2968783?v=4",
  "gravatar_id": "",
  "url": "https://api.github.com/users/Philon",
  ...
  "public_repos": 4,
  "public_gists": 0,
  "followers": 15,
  "following": 0,
  "created_at": "2012-12-05T06:28:25Z",
  "updated_at": "2019-03-06T12:47:11Z"
}
```

多句嘴，我觉得GitHub的RESTful设计得相当不错！

可以看到GitHub返回了我的个人账户信息，并且是json格式，如果在GO程序中同样可以采用`http.Get()`函数获取到这些信息，只可惜get到的全部是一对字符串，如何将其变成一个便于处理和调用的数据结构呢？两种方式：

**方式一、类型映射**
```go
// User Github用户账户信息
// 每个变量最后用反引号标示的字符串是——标签
// 标签可为之后的json.Decoder提供映射依据
type User struct {
  Login             string `json:"login"`
  ID                int64  `json:"id"`
  AvatarURL         string `json:"avatar_url"`
  GravatarID        string `json:"gravatar_id"`
  URL               string `json:"url"`
  HTMLURL           string `json:"html_url"`
  FollowersURL      string `json:"followers_url"`
  FollowingURL      string `json:"following_url"`
  GistsURL          string `json:"gists_url"`
  StarredURL        string `json:"starred_url"`
  SubscriptionsURL  string `json:"subscriptions_url"`
  OrganizationsURL  string `json:"organizations_url"`
  ReposURL          string `json:"repos_url"`
  EventsURL         string `json:"events_url"`
  RecievedEventsURL string `json:"recieved_events_url"`
  Type              string `json:"type"`
  SiteAdmin         bool   `json:"site_admin"`
  Name              string `json:"name"`
  Company           string `json:"company"`
  Blog              string `json:"blog"`
  Location          string `json:"location"`
  Email             string `json:"email"`
  Hireable          string `json:"hireable"`
  Bio               string `json:"bio"`
  PublicRepos       int32  `json:"public_repos"`
  PublicGists       int32  `json:"public_gists"`
  Followers         int32  `json:"followers"`
  Following         int32  `json:"following"`
  CreateAt          string `json:"create_at"`
  UpdateAt          string `json:"update_at"`
}

// DeserializeToType 获取用户信息，并反序列化为User类型
func DeserializeToType(name string) {
  // 根据用户名从GitHub获取对应用户的json信息
  resp, err := http.Get("https://api.github.com/users/" + name)
  if err != nil {
  log.Println("ERROR: ", err)
  }

  defer resp.Body.Close()

  var user User
  // 通过Decode将响应内容反序列化为对象
  err = json.NewDecoder(resp.Body).Decode(&user)
  if err != nil {
  log.Println("ERROR: ", err)
  }

  fmt.Printf("user: %v\n", user)
}
```

以上的代码很长，但归根结底就两个部分：
1. 在struct类型定义中，通过`name type tag`的标准格式定义结构类型中的每个属性，注意最后反引号框起来的标签，它用于之后给json反序列化提供映射依据——如果仔细比对就会发现，每个tag中的名称都和GitHub响应返回的json中的键名严格一致。
2. DeserializeToType函数其实就是具体的反序列化过程了，只需要记住`json.NewDecoder().Decode(&user)`这个标准库中的函数即可，该函数会根据第1部分的tag将json数据解析后填入对象中。

**方式二、字典映射**

有的时候，其实没必要为每个json消息都定义类型，不然得累死，所以GO提供了一种更为灵活的方式——字典。还是以获取GitHub用户信息为例：
```go
func DeserializeToMap(name string) {
  resp, err := http.Get("https://api.github.com/users/" + name)
  if err != nil {
    log.Fatalln("ERROR: ", err)
  }

  defer resp.Body.Close()

  // 读取响应Body并转化为[]byte数据结构
  content, err := ioutil.ReadAll(resp.Body)
  if err != nil {
    log.Fatalln("ERROR: ", err)
  }

  var user map[string]interface{}
  // 通过Unmarshal将[]byte转换为字典
  err = json.Unmarshal(content, &user)
  if err != nil {
    log.Fatalln("ERROR: ", err)
  }

  fmt.Println("Username: ", user["login"])
  fmt.Println("Followers: ", user["followers"])
}
```

在上述代码中，先通过`ioutil.ReadAll`将服务器端的响应读取出来并转换为`[]byte`字节流形式，然后在通过`json.Unmarshal`把这个字节流转换成`map[string]interface{}`的数据字典形式，注意字典的类型必须是这种，不要随意更换。

如果要根据json中的键来获取值信息就非常简单了，就是上述代码的最后两行`user["key"]`即可。

### 序列化

明白了反序列化的处理方式，还要懂得序列化，毕竟请求/应答不分家，那么如何将一个具体的数据结构转化为json字符串呢？方式只有一种`json.Marshal`，但要注意数据类型一般是两种：对象和字典。

类型对象的序列化：
```go
func SerializeType() {
  user := &User{
    Login: "张三",
    ID:    9527,
    URL:   "https://api.github.com/users/ZhangSan",
  }

  // 带缩进格式的序列化，缩进为4个空格
  data, err := json.MarshalIndent(user, "", "    ")
  if err != nil {
    log.Fatalln("ERROR: ", err)
  }

  fmt.Println(string(data))
}
```

上述代码定义了一个`User`对象，并通过`json.MarshalIndent`非常容易就将其转换为data字节流，为了输出我们把它强转为string类型。

留意一下，`func MarshalIndent(v interface{}, prefix, indent string) `函数是带格式化的转换，也就是说，默认情况下，json字符串其实没有空格和换行的，这个函数可以根据你的喜好从新将其格式化。

字典的序列化：
```go
func SerializeMap() {
  c := make(map[string]interface{})
  c["name"] = "张三"
  c["id"] = 9527

  // 不带缩进格式化的序列化
  data, err := json.Marshal(c)
  if err != nil {
    log.Fatalln("ERROR: ", err)
  }

  fmt.Println(string(data))
}
```

字典无需过多重复，和反序列化那部分基本一样。

## 输入输出

关于`io.Reader`和`io.Writer`就我个人而言，没有太多值得牢记的地方，最多也就一句话：**但凡实现io.Reader/Writer接口的类型，都可以被标准库中的io调用**。

以书中的例子来说：
```go
func main() { 
	// 创建一个Buffer值，并将一个字符串写入Buffer 
	// 使用实现io.Writer的Write方法 
	var b bytes.Buffer
	b.Write([]byte("Hello "))

	// 使用Fprintf来将一个字符串拼接到Buffer里 
	// 将bytes.Buffer的地址作为io.Writer类型值传入 
	fmt.Fprintf(&b, "World!")

	// 将Buffer的内容输出到标准输出设备 
	// 将os.File值的地址作为io.Writer类型值传入 
	b.WriteTo(os.Stdout)
} 
```

上述代码中，首先Buffer类型实现了Write方法，所以变量b可以被标准的fmt.Fprintf接受。而`Buffer.WriteTo(*File)`函数是写到一个文件中，`os.Stdout`就是标准输出文件。

`Reader.Read()`接口几乎也是同样的道理，只要实现Read接口的类型，都可以被标准库接受。

我觉得书中本章最开始的话也充分反映了GO语言的思想：**GO开发者会比其它语言的开发者更依赖标准库里的包**。为什么要非常熟悉GO语言的标准库，为什么要充分掌握其接口的实现原则。从上边的三个例子中就可以看出，每当我们需要增加新业务时，完全可以仅实现标准库中的某个接口，就可以近乎完美地衔接进整个GO生态。

比如我为某个结构类型实现了`Writer.Write()`接口，我几乎可以肯定，这个类型同时“继承”了log、json包里的诸多功能。

## 小结一下

- 标准库有特殊的保证，并且被社区广泛应用。
- 使用标准库的包会让代码易于管理，更加受信任。
- 标准库放在$GOROOT/pkg下，以静态库形式存放。
- log.Logger可以定制自己的日志形式。
- json包可以通过结构类型的标签，实现序列化和反序列化
- map[string]interface{}也可以用于json编解码
- 接口允许代码组合已有的功能，得接口者得全标准库
- 熟悉标准库！熟悉标准库！熟悉标准库！
