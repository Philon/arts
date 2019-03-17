# GO语言实战》学习笔记九：测试和性能

在很多软件开发中，单元测试是最为基础且最有效的软件质量保证手段。我个人是搞C语言开发出身，想当年，`printf`打天下，从来就没怕过谁。而后来逐步接触C#/Java等企业级的编程语言，才明白单元测试对功能模块和业务的重要性，加之现在IDE的强悍，查错效率也是极高的。

GO语言也提供了相对完善的测试框架——testing包，其实这类内容网上一搜一大把，作者却将其作为本书的最后一章单独提出来，想必他也清楚“测试”对软件开发而言的地位。

所以，就我个人而言，最后一章不难，主要是学习如何全方位地做软件测试，即`testing`包和`go test`命令的使用，这些内容很重要，包括:
- 如何创建单元测试
- 如何模拟生产环境
- 如何测试性能

## GO语言单元测试

1. 确保文件名为`xxx_test.go`的形式
2. 确保单元测试函数为`TestXXX(t *testing.T)`的形式
3. 使用`go test`直接运行所有的测试文件

此外，本书还按场景提供了不同的测试方法，主要有：
- **基础单元测试**：最常规的，按照预期值测试
- **表组测试**：多个输入值，多个预期值测试
- **模仿调用**：本地模拟服务端，排除网络问题，仅测试业务
- **端点测试**：针对类似RESTful结构，测试某个单一路径功能

为了说明这些测试是如何实现和使用的，我们需要先声明两个全局变量，后续的所有代码中都会调用：
```go
// 如果测试通过，在行尾打✅
const checkMark = "\u2713"

// 如果测试失败，在行尾打❌
const ballotX = "\u2717"
```

### 1.基础单元测试

以下测试是按照书中的举例写的，主要是对某个url发起http请求，正常情况下服务端都会响应200表示OK，但也可能会出现404找不到，或者干脆连接超时的情况出现。这段代码中的url是书中提供的，**会出现404或超时**：
```go
func TestSingle(t *testing.T) {
	url := "http://www.goinggo.net/feeds/posts/default?alt=rss"
	statusCode := 200

	t.Log("测试下载: ", url)
	{
		t.Logf("\t预期收到状态码为: %d", statusCode)
		{
			resp, err := http.Get(url)
			if err != nil {
				t.Fatal("\t\t调用http.Get()发起请求失败: ", ballotX, err)
			}
			t.Log("\t\t调用http.Get()发起请求", checkMark)

			defer resp.Body.Close()
			if resp.StatusCode == statusCode {
				t.Logf("\t\t收到状态码 %v %v", resp.StatusCode, checkMark)
			} else {
				t.Errorf("\t\t收到状态码 %v %v", resp.StatusCode, ballotX)
			}
		}
	}
}

// ---------- go test -v ----------
=== RUN   TestSingle
--- FAIL: TestSingle (3.69s)
    unit_test.go:18: 测试下载:  http://www.goinggo.net/feeds/posts/default?alt=rss
    unit_test.go:20:    预期收到状态码为: 200
    unit_test.go:26:            调用http.Get()发起请求 ✓
    unit_test.go:32:            收到状态码 404 ✗
```

如上述代码可以看到，所谓**单元测试，主要是通过执行某些过程，比对其结果是否符合预期**。这里发起http请求只是过程，而**断言**`http.Get()`函数会成功以及服务端响应200是预期。从执行结果可以看到，请求成功了，但服务端响应状态码为404，不符合预期，测试失败。

注意`t.Log`的使用，基本是从log包定制的一套日志实例，主要是能在每行日志前增加测试的源文件及行号，便于错误定位。

另外，源码中每个`t.Log`后都带有一对大括号，这个不是必须的，因为几乎每个日志内容里都含有`\t`缩进符，估计作者的本意是为了直观地表示缩进吧。

### 2.表组测试

很多时候我们都需要用大量且不同的参数来测试某个函数的执行结果是否都符合预期，而这就是表组测试。其实表组测试没有什么特别的地方，无非就是**把测试参数装进数组里，通过遍历测试每一个**。

```go
func TestMulti(t *testing.T) {
	urls := []struct {
		url  string
		code int
	}{
		{
			"https://www.github.com",
			http.StatusOK, // 200
		}, {
			"https://www.github.com/philon/123",
			http.StatusNotFound, // 404
		},
	}

	t.Log("测试访问一组URL，并检查状态码是否正确")
	{
		for _, u := range urls {
			t.Logf("\t对'%s'发起请求，预期状态码为: %d", u.url, u.code)
			{
				resp, err := http.Get(u.url)
				if err != nil {
					t.Fatal("\t\t发起请求失败: ", err, ballotX)
				}
				t.Log("\t\t发起请求成功", checkMark)

				defer resp.Body.Close()
				if resp.StatusCode == u.code {
					t.Log("\t\t收到状态码: ", resp.StatusCode, checkMark)
				} else {
					t.Log("\t\t收到状态码: ", resp.StatusCode, ballotX)
				}
			}
		}
	}
}

// ---------- go test -v ----------
=== RUN   TestMulti
--- PASS: TestMulti (1.77s)
    unit_test.go:52: 测试访问一组URL，并检查状态码是否正确
    unit_test.go:55:    对'https://www.github.com'发起请求，预期状态码为: 200
    unit_test.go:61:            发起请求成功 ✓
    unit_test.go:65:            收到状态码:  200 ✓
    unit_test.go:55:    对'https://www.github.com/philon/123'发起请求，预期状态码为: 404
    unit_test.go:61:            发起请求成功 ✓
    unit_test.go:65:            收到状态码:  404 ✓
```

如上，**把多个测试参数放进一个[]struct{}形式的切片中，并通过for-range循环遍历测试**，就是表组测试的最基本用法。剩下的内容和第一节的单元测试没什么不同。

### 3.模仿调用

前两个例子一直是对某些网站发起请求，如果对面的服务器挂了怎么办？或者我们的服务器根本就还没上线怎么办？模仿调用就是**模拟服务端**的意思，模拟出一个服务器，对其发起请求，主要测试业务逻辑是否正常。

```go
// mockServer 创建虚拟http服务
// 默认响应200，并返回一段json数据
func mockServer() *httptest.Server {
  f := func(w http.ResponseWriter, r *http.Request) {
    w.WriteHeader(200)
    w.Header().Set("Content-Type", "application/json")
    fmt.Fprintln(w, `{"id": 9527, "login": "philon"}`)
  }

  return httptest.NewServer(http.HandlerFunc(f))
}

func TestMocking(t *testing.T) {
...
  // 在测试函数中将模拟服务器启动
  server := mockServer()
  defer server.Close()
  // 通过server.URL可访问到模拟服务器
  resp, err := http.Get(server.URL)
...
}
```

这段代码没有完，因为除了模拟http服务端的部分，其它都会基础单元测试一样，就不重复了。**http服务端模拟主要通过httptest包实现的**，把这部分用法搞清楚即可。

### 4.端点测试

众所周知，RESTFul的基本设计思想就是通过URL资源访问，并以`GET|POST|PUT|DELETE|PATCH`等请求方法区别不同的业务逻辑，比如：
- GET /users 获取用户列表
- PATCH /users/philon/profile 更新用户配置
- DELETE /users/philon 删除指定用户

所谓的端点也就是**访问路径**的意思，比如/users、/users/philon这样的路径，并针对不同的请求方法进行测试。

**自建http服务端**

要完成这部分的演示，需要先自建一个RESTFul的服务端，或者说传统的MVC架构的服务，为了简单说明，这里仅仅实现`GET /users`获取用户列表的功能。

```go
// Routes 全局路由映射
func Routes() {
	http.HandleFunc("/users", Users)
}

// Users 用户列表控制器
func Users(rw http.ResponseWriter, r *http.Request) {
	list := []struct {
		ID   int    `json:"id"`
		Name string `json:"username"`
	}{
		{1234, "张三"},
		{4567, "李四"},
		{5678, "王五"},
	}

	rw.Header().Set("Content-Type", "application/json")
	rw.WriteHeader(http.StatusOK)
	json.NewEncoder(rw).Encode(&list)
}

func main() {
	Routes()
	log.Println("Http server start listening: ", 4000)
	http.ListenAndServe(":4000", nil)
}
```

上面这段代码通过`go run main.go`将该服务启动后，可以通过浏览器直接访问`http://localhost:4000/users`即可看到结果，这里用curl请求也一样：
```sh
$ curl -i localhost:4000/users

HTTP/1.1 200 OK
Content-Type: application/json
Date: Fri, 15 Mar 2019 00:18:13 GMT
Content-Length: 98

[{"id":1234,"username":"张三"},{"id":4567,"username":"李四"},{"id":5678,"username":"王五"}]
```

如此这般，一个最简单的RESTFul设计风格的http服务端就做好了。但是！！这并不是端点测试的全部，注意`Routes()`函数里的`http.HandleFunc("/users", Users)`，这才是路由功能的实现，将路径`/users`只想Users函数。设想一下，如果你把Users相关的服务端的业务代码写完，你会如何测试？搭建http环境——跑服务代码——跑客户端请求代码——看结果？NO，效率太低了。因为只是测试Users函数有没有正确返回json数据，所以可以仿照`3.模仿调用`的方式虚拟一个http服务器，直接去测试`/users`路径。

```go
func init() {
	// 初始化路径，给端点测试用
	Routes()
}

func TestController(t *testing.T) {
	t.Log("测试服务端点")
	{
		req, err := http.NewRequest("GET", "/users", nil)
		if err != nil {
			t.Fatal("\t创建请求对象失败 ", ballotX, err)
		}
		t.Log("\t创建请求对象成功 ", checkMark)

		// httptest创建虚拟服务器
		rw := httptest.NewRecorder()
		http.DefaultServeMux.ServeHTTP(rw, req)

		if rw.Code != http.StatusOK {
			t.Fatalf("\t收到状态码 %d 不符合预期 %v", rw.Code, ballotX)
		}
		t.Log("\t收到状态码", rw.Code, checkMark)

		users := []struct {
			ID   int    `json:"id"`
			Name string `json:"username"`
		}{}

		if err := json.NewDecoder(rw.Body).Decode(&users); err != nil {
			t.Fatal("响应不是json类型的数据 ", ballotX)
		}
		t.Log("\tJSON反序列化成功 ", checkMark)

		if len(users) == 3 {
			t.Log("\t用户列表长度检查 ", checkMark)
		} else {
			t.Log("\t用户列表长度检查 ", ballotX)
		}
	}
}
```

## 小结一下

- GO语言自带测试框架testing包
- go test用来运行测试
- 测试文件必须以_test.go结尾
- 测试有单元测试、表组测试、模拟测试、端点测试