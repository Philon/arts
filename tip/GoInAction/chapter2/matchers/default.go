package matchers

import "../search"

type defaultMatcher struct{}

func (matcher defaultMatcher) Search(feed *search.Feed, keyword string) ([]*search.Result, error) {
	return nil, nil
}

func init() {
	var matcher defaultMatcher
	search.Register("default", matcher)
}
