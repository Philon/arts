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

// GetbUser 获取github的用户信息
func GetUser(name string) {
	resp, err := http.Get("https://api.github.com/users/" + name)
	if err != nil {
		log.Println("ERROR: ", err)
	}

	defer resp.Body.Close()

	var user User
	err = json.NewDecoder(resp.Body).Decode(&user)
	if err != nil {
		log.Println("ERROR: ", err)
	}

	fmt.Printf("user: %v\n", user)
}

// GetUserMap 获取用户信息，并转换为字典
func GetUserMap(name string) {
	resp, err := http.Get("https://api.github.com/users/" + name)
	if err != nil {
		log.Fatalln("ERROR: ", err)
	}

	defer resp.Body.Close()

	content, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		log.Fatalln("ERROR: ", err)
	}

	var user map[string]interface{}
	err = json.Unmarshal(content, &user)
	if err != nil {
		log.Fatalln("ERROR: ", err)
	}

	fmt.Println("Username: ", user["login"])
	fmt.Println("Followers: ", user["followers"])
}
