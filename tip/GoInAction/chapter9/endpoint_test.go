package main

import (
	"encoding/json"
	"net/http"
	"net/http/httptest"
	"testing"
)

func init() {
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
