// 展示如何编写一个“基础测试”
package main

import (
	"net/http"
	"testing"
)

const checkMark = "\u2713" // ✓符号
const ballotX = "\u2717"   // ✗符号

// TestSingle 测试http包的Get函数是否下载了内容
// 所有单元测试都应该是TestName(t *testing.T)的形式
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
