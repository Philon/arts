package main

import (
	"fmt"
	"net/http"
	"net/http/httptest"
	"testing"
)

// mockServer 创建虚拟http服务
func mockServer() *httptest.Server {
	f := func(w http.ResponseWriter, r *http.Request) {
		w.WriteHeader(200)
		w.Header().Set("Content-Type", "application/json")
		fmt.Fprintln(w, `{"id": 9527, "login": "philon"}`)
	}

	return httptest.NewServer(http.HandlerFunc(f))
}

// TestMocking 模拟请求测试，自建一个虚拟http服务器请求
// 这种情况主要用于网络不同时，测试相关业务处理是否正常
func TestMocking(t *testing.T) {
	statusCode := http.StatusOK

	server := mockServer()
	defer server.Close()

	t.Log("模拟测试请求")
	{
		t.Logf("\t对'%s'发起请求，预期状态码为: %d", server.URL, statusCode)
		resp, err := http.Get(server.URL)
		if err != nil {
			t.Fatal("\t发起请求失败: ", err, ballotX)
		}
		t.Log("\t发起请求成功 ", checkMark)
		defer resp.Body.Close()

		if resp.StatusCode == http.StatusOK {
			t.Log("\t收到状态码: ", resp.StatusCode, checkMark)
		} else {
			t.Log("\t收到状态码: ", resp.StatusCode, ballotX)
		}
	}
}
