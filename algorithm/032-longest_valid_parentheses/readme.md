# [最长有效括号](https://leetcode.com/problems/longest-valid-parentheses/)

给一个仅包含`'('`和`')`的字符串，找出最长有效(合式)括号的子串长度。

```
Example 1:
Input: "(()"
Output: 2
Explanation: 最长有效括号子串是"()"

Example 2:
Input: ")()())"
Output: 4
Explanation: 最长有效括号子串是"()()"
```

## 概要

我们需要查明给定字符串中最大有效括号子串的长度。

## 解决方案

### 方案一、暴力

**算法**

在这方案中，我们要找出字符串中每一个非空子串的长度，然后检查它们是否为有效括号字符串。为了检测有效性，我们要用到栈方法。

每次遇到'('我们就将其压栈，每次遇到')'我们就出栈一个'('。如果任何时候已经没有'('在栈里了，或者遍历完子串后栈里还有'('元素，那这个子串就不是有效的。在这种方法下，我们要重复去处理每个可能的子串，并存储已找到的最长有效子串。

```
Example:
"((())"

(( --> invalid
(( --> invalid
() --> valid, length=2
)) --> invalid
((()--> invalid
(())--> valid, length=4
maxlength=4
```

```java
public class Solution {
    public boolean isValid(String s) {
        Stack<Character> stack = new Stack<Character>();
        for (int i = 0; i < s.length(); i++) {
            if (s.charAt(i) == '(') {
                stack.push('(');
            } else if (!stack.empty() && stack.peek() == '(') {
                stack.pop();
            } else {
                return false;
            }
        }
        return stack.empty();
    }
    public int longestValidParentheses(String s) {
        int maxlen = 0;
        for (int i = 0; i < s.length(); i++) {
            for (int j = i + 2; j <= s.length(); j+=2) {
                if (isValid(s.substring(i, j))) {
                    maxlen = Math.max(maxlen, j - i);
                }
            }
        }
        return maxlen;
    }
}
```

- 时间复杂度：O(n³)，生成所有可能的子串需要O(n²)，每个子串有效性检查又需要遍历其长度O(n)。
- 空间复杂度：O(n)，栈的深度相当于最长有效子串的长度n。

### 方案二、动态规划

**算法**

这个问题可以通过动态规划解决。我们使用一个**dp**数组，其中第i个元素的索引相当于最长有效子串的长度。我们将dp数组初始化为0。现在可以明确的是子串必须以')'结尾。进一步可以推导出结论，以'('结尾的子串总是在它们对应dp的索引处包含0。因此，我们尽在遇到')'时才更新dp数组。

为了填满dp数组，我们每次只检查字符串中的两个连续字符，如果：
1. s[i]=')'并且s[i-1]='('，即字符串看起来像"...()" =>  
   dp[i]=dp[i−2]+2  
   我们这么做是因为不管怎样"()"部分是一个有效的子串，并导致前一个有效子串的长度增加2。
2. s[i]=‘)’并且s[i−1]=‘)’，即字符串看起来像"...))" =>  
   如果 s[i−dp[i−1]−1]=‘(’ 则 dp[i]=dp[i−1]+dp[i−dp[i−1]−2]+2  
   这背后的原因是，如果第二个')'收尾的子串是有效的(成为subs)，那么它会成为更大子串的一部分，肯定有一个与它对应的'('在这个有效子串之前。因此，如果在subs之前的符号是'('，我们就更新dp[i]为subs的长度，在dp[i-1]的基础上加2。

```java
public class Solution {
  public int longestValidParentheses(String s) {
    int maxans = 0;
    int dp[] = new int[s.length()];
    for (int i = 1; i < s.length(); i++) {
      if (s.charAt(i) == ')') {
        if (s.charAt(i - 1) == '(') {
          dp[i] = (i >= 2 ? dp[i - 2] : 0) + 2;
        } else if (i - dp[i - 1] > 0 && s.charAt(i - dp[i - 1] - 1) == '(') {
          dp[i] = dp[i - 1] + ((i - dp[i - 1]) >= 2 ? dp[i - dp[i - 1] - 2] : 0) + 2;
        }
        maxans = Math.max(maxans, dp[i]);
      }
    }
    return maxans;
  }
}
```

- 时间复杂度：O(n)，遍历了一遍字符串用于填满dp。
- 空间复杂度：O(n)，dp数字的长度。

### 方案三、栈

为了替代(方案一)找出每个可能的字符串并检查有效性，我们可以用到栈来检查目前为止扫描过的字符串的有效性，以及字符串的长度。为了达成次目的，我们会一开始push一个-1到栈里。

对于遇到的每个'('，我们都push它的索引到栈里。

对于遇到的每个')'，我们都pop出顶部的元素，然后再用当前元素索引减去栈顶的元素，可表示当前遇到的有效括号字符串的长度。如果栈去的元素弹出到空了的时候，我们push当前元素的索引进去。在这样的方式下，我们总能计算出有效子串的长度，并最终返回最大有效字符串的长度。

```java
public class Solution {
    public int longestValidParentheses(String s) {
        int maxans = 0;
        Stack<Integer> stack = new Stack<>();
        stack.push(-1);
        for (int i = 0; i < s.length(); i++) {
            if (s.charAt(i) == '(') {
                stack.push(i);
            } else {
                stack.pop();
                if (stack.empty()) {
                    stack.push(i);
                } else {
                    maxans = Math.max(maxans, i - stack.peek());
                }
            }
        }
        return maxans;
    }
}
```
- 时间复杂度：O(n)，n表示给定字符串的长度。
- 空间复杂度：O(n)，栈区的长度。

### 方案四、不用额外空间

**算法**

在这个方案里，我们使用两个计数器`left`和`right`。首先，我们从左到右遍历一遍字符串，每遇到'('时left计数器增加，每遇到')'时right计数器增加。当left等于right时，我们可以计算出当前有效字符串的长度，并比较出直到当前最大长度的子串。如果right大于left，我们就重置二者为0。

下一步，我们从右到左按照相似的过程遍历一遍字符串。

```java
public class Solution {
    public int longestValidParentheses(String s) {
        int left = 0, right = 0, maxlength = 0;
        for (int i = 0; i < s.length(); i++) {
            if (s.charAt(i) == '(') {
                left++;
            } else {
                right++;
            }
            if (left == right) {
                maxlength = Math.max(maxlength, 2 * right);
            } else if (right >= left) {
                left = right = 0;
            }
        }
        left = right = 0;
        for (int i = s.length() - 1; i >= 0; i--) {
            if (s.charAt(i) == '(') {
                left++;
            } else {
                right++;
            }
            if (left == right) {
                maxlength = Math.max(maxlength, 2 * left);
            } else if (left >= right) {
                left = right = 0;
            }
        }
        return maxlength;
    }
}
```

- 时间复杂度：O(n)，遍历了两遍字符串用。
- 空间复杂度：O(1)，仅仅用到了left和right变量的空间。