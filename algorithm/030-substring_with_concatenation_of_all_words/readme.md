# [算法标题](https://leetcode.com/problems/substring-with-concatenation-of-all-words/)

给你一个字符串——`s`，以及一个单词列表——`words`，每个单词长度相同。找出`s`中所有将`words`中的每个单词串联起来的子串的起始索引，串联的单词中间没有任何多余字符。
```
Example 1:
Input:
  s = "barfoothefoobarman",
  words = ["foo","bar"]
Output: [0,9]
Explanation: 子串的起始索引是0和9，分别是"barfoor"和"foobar"。
不用对输出排序，返回[9,0]也是一样的。

Example 2:
Input:
  s = "wordgoodgoodgoodbestword",
  words = ["word","good","best","word"]
Output: []
```

## 我的思路

