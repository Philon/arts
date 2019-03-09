// 利用标准库中的http、json等库，实现从github官网获取用户信息
package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
)

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

// ----------json解码----------

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

// DeserializeToMap 获取用户信息，并转换为字典
// GO语言中的map即数据字典的意思
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

// ----------json编码----------

// SerializeType 将User类型序列化为json字符串
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

// SerializeMap 将字典map序列化为json字符串
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
