package main

import (
	"encoding/json"
	"log"
	"net/http"
)

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
