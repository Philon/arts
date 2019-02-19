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

这道题可能对我来说太困难了，其实除了words列表里有副本的测试用例，大体上还是做出来了。

考虑到本周arts因病延迟提交两天了，我决定推迟解决该题！

关于本题我的思路基本还固定在遍历words来匹配字符串，而查看了论坛里其他人基本是用哈希表+滑窗算法解决的。(可能这两天脑子烧出问题了，感觉自己正在钻牛角尖中...先做去别的，重启大脑)
