package main

import (
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"net/http/httptest"
)

// ExampleUsers 用户列表json数据获取示例
func ExampleUsers() {
	r, _ := http.NewRequest("GET", "/users", nil)
	rw := httptest.NewRecorder()
	http.DefaultServeMux.ServeHTTP(rw, r)

	var users []struct {
		ID   int    `json:"id"`
		Name string `json:"username"`
	}

	if err := json.NewDecoder(rw.Body).Decode(&users); err != nil {
		log.Println("ERROR: ", err)
	}

	fmt.Println(users)
}
