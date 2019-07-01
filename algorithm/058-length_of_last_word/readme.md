# [末尾单词长度](https://leetcode.com/problems/length-of-last-word/)

给一个字符串，由大小写字母表和空格组成，返回最后一个单词的字符串长度。如果最后一个单词不存在，返回0。

注意：一个单词是指由非空格字符定义的字符序列。

```
Example:
Input: "Hello World"
Output: 5
```

## 解题思路

从字符串末尾开始往前遍历，直到遇到第一格空格，不过要留意去除末尾的所有空格。

```cpp
class Solution {
public:
  int lengthOfLastWord(string s) {
    const char* p = s.c_str();
    const char* q = p + s.length() - 1;
    int lastLength = 0;

    while (*q == ' ') q--;
    while (q >= p && *q != ' ') {
      q--;
      lastLength++;
    }

    return lastLength;
  }
```
- 空间复杂度：O(n)，n取决于末尾空格+末尾单词长度
- 时间复杂度：O(1)