# [实现strStr()](https://leetcode.com/problems/implement-strstr/)

实现`strStr()`函数。

返回第一次出现的指针索引，否则返回-1.

```
Example 1:
Input: haystack = "hello", needle = "ll"
Output: 2

Example 2:
Input: haystack = "aaaaa", needle = "bba"
Output: -1
```

澄清一下:

当needle为空字符串时应该返回什么？这在面试中是一个很好的问题。

就本题而言，当needle为空时，我们返回0。这也是C语言中的`strstr()`和Java中的`indexOf()`的一贯做法。

## 我的思路

1. 先遍历haystack找到第一个与needle第一个相同的字符
2. 继续遍历haystack和needle，尝试找到第一个与不相同的字符
3. 如果找到了，说明不是子串，将haystack回退到之前相同处的下一个，重复第1步
4. 如果needle遍历完都没找到不同，说明就是子串，返回第一个相同处

以下是C实现：
```c
int strStr(char* haystack, char* needle) {
  char* p = haystack;
  while (*needle) {
    char* q = needle;
    while (*p && *p != *q) p++;
    
    int i = 0;
    while (p[i] && p[i] == q[i]) i++;

    if (q[i] == '\0') {
      return p - haystack;
    } else if (p[i] == '\0' && q[i]) {
      return -1;
    } else {
      p++;
    }
  }

  return 0;
}
```