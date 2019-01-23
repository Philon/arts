# [生成括号](https://leetcode.com/problems/generate-parentheses/)

输出n对括号，写一个函数用于生成所有可闭合的括号组合。
	
例如，设`n = 3`，其结果就是：
```
[
    "((()))",
    "(()())",
    "(())()",
    "()(())",
    "()()()"
]
```

## 我的思路：回溯法

这道题其实我没有自己的想法，主要是看了官方解答之后，大致了解了[回溯法](https://zh.wikipedia.org/wiki/回溯法)，以及[八皇后问题](https://juejin.im/post/5accdb236fb9a028bb195562)，觉得比较经典，所以重新按照这种递归套路把官方解答的`方案二`实现了一遍。

要解决这道题需要记住要让括号对组合总是合法，必须遵循两个逻辑：  
1. 开括号`'('`的数量不得超过n
2. 闭括号`')'`当前数量不得超过开括号

然后，可以套用八皇后的解题思路，一个括号一个括号的放，每放下一个括号之前，先用上面的两个逻辑验证应该放`'('`还是放`')'`。

回溯法的本质就是递归，从字符串的角度来看，是从左到右的顺序在放置括号，每放置一个括号都是一层递归(需要一点空间想象力)。

```c
void generate(char*** list, int* size, char* current, int open, int close, int n) {
  int i = open + close;
  if (i == (2 * n)) {
    // 如果括号放置完毕，在列表中追加存储区域，保存新字符串
    int l = *size;
    char** p = (char**)realloc(*list, sizeof(char*) * (l + 1));
    p[l] = (char*)calloc(2 * n + 1, sizeof(char));
    strcpy(p[l], current);

    *list = p;
    *size = l + 1;
    return;
  }

  // 根据当前需要放置括号的位置，决定该放置开还是闭括号
  if (open < n) {
    current[i] = '(';
    generate(list, size, current, open + 1, close, n);
  }

  if (close < open) {
    current[i] = ')';
    generate(list, size, current, open, close + 1, n);
  }
}

char** generateParenthesis(int n, int* returnSize) {
  char** parentheses = NULL;
  char* queue = (char*)calloc(2 * n + 1, sizeof(char));
  generate(&parentheses, returnSize, queue, 0, 0, n);

  return parentheses;
}
```
时间/空间复杂度参考下文的方案二。

---
以下是官方答案

## 方案一、蛮力

我们可以先生成2的2n次方个'('和')'字符的括号对。然后一一检查哪些是无效的。

### 算法

为了生成所有的括号，我们需要递归。所有括号对的长度n相当于，长度为n-1个`'('`，加上长度为n-1个`')'`。

为了检查每种括号组合是否合法，我们要跟踪剩余部分。开括号的数量减去闭括号的数量总是为零，否则不合法。

```java
class Solution {
    public List<String> generateParenthesis(int n) {
        List<String> combinations = new ArrayList();
        generateAll(new char[2 * n], 0, combinations);
        return combinations;
    }

    public void generateAll(char[] current, int pos, List<String> result) {
        if (pos == current.length) {
            if (valid(current))
                result.add(new String(current));
        } else {
            current[pos] = '(';
            generateAll(current, pos+1, result);
            current[pos] = ')';
            generateAll(current, pos+1, result);
        }
    }

    public boolean valid(char[] current) {
        int balance = 0;
        for (char c: current) {
            if (c == '(') balance++;
            else balance--;
            if (balance < 0) return false;
        }
        return (balance == 0);
    }
}

```
- 时间复杂度：O(2的2n次方 * n)
- 空间复杂度：同上

## 方案二、回溯法

不要像方案一那样每次都相加`'('和')'`，只有在确定该序列的组合有效时再进行相加。我们可以通过跟踪放置开/闭括号的位置的数量来达到这一点。如果我们还剩下一个(或n个位置)，我们可以接着放开括号。如果它没有超过开括号，我们可以放闭括号。

```java
class Solution {
    public List<String> generateParenthesis(int n) {
        List<String> ans = new ArrayList();
        backtrack(ans, "", 0, 0, n);
        return ans;
    }

    public void backtrack(List<String> ans, String cur, int open, int close, int max){
        if (cur.length() == max * 2) {
            ans.add(cur);
            return;
        }

        if (open < max)
            backtrack(ans, cur+"(", open+1, close, max);
        if (close < open)
            backtrack(ans, cur+")", open, close+1, max);
    }
}
```

## 方案三、闭包号(Closure Number)

为了枚举某些内容，我们通常希望将其表示为不相交子集会更容易计算其总和。

考虑一个有效括号序列`S`的闭包号：最小 `index > = 0`，所以`S[0]，S[1] ... S[2*index+1]`是有效的。 显然，每个括号序列都有一个唯一的闭包号。 我们可以尝试单独列举它们。

### 算法

给每个闭包号设置为`c`，我们知道起始/结束的括号索引分别为`0`和`2*c+1`，然后在其中的`2*c`的括号元素必须是有效的组合，把这些元素加起来就行了。

```java
class Solution {
    public List<String> generateParenthesis(int n) {
        List<String> ans = new ArrayList();
        if (n == 0) {
            ans.add("");
        } else {
            for (int c = 0; c < n; ++c)
                for (String left: generateParenthesis(c))
                    for (String right: generateParenthesis(n-1-c))
                        ans.add("(" + left + ")" + right);
        }
        return ans;
    }
}
```
