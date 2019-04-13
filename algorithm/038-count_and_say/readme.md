# [数数](https://leetcode.com/problems/count-and-say/)

计数与读序列就类似于以下5项的整数序列：
1. 1
2. 11
3. 21
4. 1211
5. 111221

1读作“一个1”或者“11”
11又读作“两个1”或者“21”
21再读作“一个2一个1”或者“1211”
...
以此类推

给定一个整数`n`范围是`1 ≤ n ≤ 30`，生成第n项的读法序列。

注意： 每项整数序列都作为一个字符串。

```
Example 1:
Input: 1
Output: "1"

Example 2:
Input: 4
Output: "1211"
```

## 我的思路一：穷举

这道题让我想起了有一道“罗马数字转阿拉伯数字”的题目，而且本题的参数值最多30个，所以...
```
char* countAndSay(int n) {
  char* terms[] = {
    "",
    "1",
    "11",
    "21",
    "1211",
    "111221",
    "312211",
    "13112221",
    "1113213211",
    "31131211131221",
    "13211311123113112211",
    "11131221133112132113212221",
    "3113112221232112111312211312113211",
    "1321132132111213122112311311222113111221131221",
    // 😂😂😂你只要有足够的毅力把30项字符串全写出来即可
  };

  return terms[n];
}
```
- 时间复杂度：O(1)
- 空间复杂度：O(1)

## 我的思路二：

第一种方案自然是开玩笑的，这道题主要麻烦在第n个数字序列其实是第n-1个序列的念法，而第n-1个序列又依赖于第n-2个，所以我们唯一能确定的是第1个数字序列念做“1”，然后顺着往下推，直到第n个。

所以第一步：实现一个根据当前数字序列“读”出下一个数字序列的函数：
```c
char* say(char* s) {
  if (s == NULL) {
    char* sequence = (char*)calloc(3, sizeof(char));
    sequence[0] = '1';
    sequence[1] = '\0';
    return sequence;
  }

  char* sequence = NULL;
  int count = 1;
  int len = 0;
  for (int i = 0; s[i] != 0; i++, count++) {
    if (s[i] != s[i+1]) {
      sequence = (char*)realloc(sequence, 3+len);
      sequence[len++] = count + '0';
      sequence[len++] = s[i];
      count = 0;
    }
  }

  free(s);
  sequence[len] = '\0';
  return sequence;
}
```

第二步：从1开始循环读，直到n为止：
```
char* countAndSay(int n) {
  char* sequence = say(NULL);
  while (--n) {
    sequence = say(sequence);
  }

  return sequence;
}
```
- 时间复杂度：O(n^m)，n是次数，m是每次数字序列的长度
- 空间复杂度：O(m)，m是第n次数字序列的长度