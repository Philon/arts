# [通配符匹配](https://leetcode.com/problems/wildcard-matching/)

给定一个字符串 (s) 和一个字符模式 (p) ，实现一个支持 '?' 和 '*' 的通配符匹配。

'?' 可以匹配任何单个字符。
'*' 可以匹配任意字符串（包括空字符串）。
两个字符串完全匹配才算匹配成功。

说明:

s 可能为空，且只包含从 a-z 的小写字母。
p 可能为空，且只包含从 a-z 的小写字母，以及字符 ? 和 *。

```
示例 1:
输入:
s = "aa"
p = "a"
输出: false
解释: "a" 无法匹配 "aa" 整个字符串。

示例 2:
输入:
s = "aa"
p = "*"
输出: true
解释: '*' 可以匹配任意字符串。

示例 3:
输入:
s = "cb"
p = "?a"
输出: false
解释: '?' 可以匹配 'c', 但第二个 'a' 无法匹配 'b'。

示例 4:
输入:
s = "adceb"
p = "*a*b"
输出: true
解释: 第一个 '*' 可以匹配空字符串, 第二个 '*' 可以匹配字符串 "dce"。

示例 5:
输入:
s = "acdcb"
p = "a*c?b"
输入: false
```

## 题目解析

本题之所以为hard，不好处理的地方主要是`'*'`匹配，即多字符模式匹配，例如：

```
s = "ab*
p = "*?"
输出为true，因为一直匹配到最后一个

s = "ab"
p = "?*"
输入为true，因为开始一直匹配到末尾

s = "ab"
p = "*?*?*"
输入为true，因为匹配了两个字符
```

根据上边的三个例子可以看到，`'*'`既可以从左往右，也可以从右往左匹配，甚至匹配长度为0，具体看`'?'`出现的情况。

这就是本题的难点，`'*'`的匹配字符数量是不确定的，需要根据之后遇到的`'?'`或`a-z`做动态调整——没错，动态规划！

## 解析思路：动态规划


