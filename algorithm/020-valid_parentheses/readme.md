# [括号匹配](https://leetcode.com/problems/valid-parentheses/)

给一个只含有`'(', ')', '{', '}', '[', ']'`的字符串，检查该字符串是否合法。

输入字符串合法性为：
1. 一个括号必须被同类型的括号闭合
2. 括号必须按顺序闭合

**注意**： 空字符串可以被认为是合法的。

```
Example 1:
Input: "()"
Output: true

Example 2:
Input: "()[]{}"
Output: true

Example 3:
Input: "(]"
Output: false

Example 4:
Input: "([)]"
Output: false

Example 5:
Input: "{[]}"
Output: true
```

## 我的思路：后进先出-LIFO

思路嘛，相对比较简单，就是设立一个缓存，只要遇上`(, [, {`的左括号，就把它给“压”进去，一旦遇到`), ], }`的右括号，就“弹”出最后一个，用于匹配比较，**第一个右括号肯定会匹配做括号**，否则就是非法。

以下是C代码：
```c
bool isValid(char* s) {
  int len = strlen(s);
  char* brackets = (char*)calloc(len + 1, sizeof(char));
  char* p = brackets;

  while (*s) {
    if (*s == '(' || *s == '[' || *s == '{') {
      *p++ = *s;
    } else {
      p--;
      if (p < brackets) return 0;
      if (*p == '(' && *s != ')') return 0;
      if (*p == '[' && *s != ']') return 0;
      if (*p == '{' && *s != '}') return 0;
    }
    s++;
  }

  free(brackets);
  return p == brackets;
}
```
- 时间复杂度：O(n)，需要遍历一遍字符串。
- 空间复杂度：O(n)，需要分配字符窜相同长度的缓存空间。

## 官方答案

> PS： 前方高能！！！居然可以把一个如此简单的问题，讲得及其复杂，佩服！

映像里，你需要在大学期间写一个编译器应付实训课，而编译器的其中一个任务(或者说子任务)就是要检测括号匹配。

结合本算法，我们将在本文看到你的编译器准备编译所有在程序中被用于处理的括号，以及检查所有的括号是否到位。于是就有了检查一个字符串中的括号是否合法，这是一个很重要的编程问题。

在该问题中，我们将处理由三种不同类型的括号：
- `()`
- `{}`
- `[]`

在考虑如何检查由括号组成的表达式是否合法之前，让我们先看一个简单版本的问题，一个由单一括号类型组成的表达式，例如：
```
(((((()))))) -- 合法

()()()()     -- 合法

(((((((()    -- 非法

((()(())))   -- 合法
```
我们用一个简单的算法来处理这个问题：
1. 我们从左边开始处理表达式的一个字符。
2. 假定我们遇到的是一个左括号，比如`(`，它可能是有效或无效的表达式，因为它有可能在表达式剩下的地方获得匹配。现在，我们简单地用个计数器对其跟踪。`left += 1`
3. 如果我们遇到一个右括号，意味着两种情况：
  1. 没有匹配，在它之前没有遇到任何右括号。或者说`left == 0`，表达式无效。
  2. 匹配，在它之前有了还没匹配过的左括号。或者说`left > 0`，表达式可能有效。
4. 如果我们遇到一个右括号，且`left == 0`，肯定无效。否则`left--`。
5. 继续重复之前的步骤，直到遍历整个字符串。
6. 如果字符串遍历完，left都不为0，那表达式肯定无效。

上述方法是通过计数器的方式实现的。但，实际的问题是，我们的括号类型有不同类型，例如遇到`]`时，我们无法确定是否有足够的`[`来匹配。你可能会说：`为什么不为每种类型的括号都设一个计数器呢？`

因为括号相对位置不同，这种方法就不奏效了。比如：`[{]`......(还有一堆文字说明，这三个括号为什么无效，我不想翻译了😂)

## 官方方案1:堆栈

一个有趣的特性，一个有效的括号表达式它的子表达式也会是有效的。(我没get到哪里有趣...)
![](https://leetcode.com/problems/valid-parentheses/Figures/20/20-Valid-Parentheses-Recursive-Property.png)

接着，如果你仔细观察上面这张图中的数据结构，就会发现一对匹配的括号被用相同的颜色标记了(官方原话😂，果然好有趣哟)。

(我不想翻译了，此处省略一堆视频、图片的文字说明，其实就是我那个思路)

### 算法
1. 初始化一个堆栈 S
2. 每次处理表达式中的一个字符
3. 如果我们遇到左括号，将其压栈。意味着我们推迟处理它，作为子表达式的头
4. 如果我们遇到右括号，S弹出一个元素，如果该元素和右括号嫩个够配对，说明有效，继续处理，否则无效
5. 直到最后，如果栈区仍然有元素，我们将其视为无效的表达式

现在，让我们共同见证算法的实现吧(官方原话)。
```java
// JAVA版本
class Solution {

  // Hash table that takes care of the mappings.
  private HashMap<Character, Character> mappings;

  // Initialize hash map with mappings. This simply makes the code easier to read.
  public Solution() {
    this.mappings = new HashMap<Character, Character>();
    this.mappings.put(')', '(');
    this.mappings.put('}', '{');
    this.mappings.put(']', '[');
  }

  public boolean isValid(String s) {

    // Initialize a stack to be used in the algorithm.
    Stack<Character> stack = new Stack<Character>();

    for (int i = 0; i < s.length(); i++) {
      char c = s.charAt(i);

      // If the current character is a closing bracket.
      if (this.mappings.containsKey(c)) {

        // Get the top element of the stack. If the stack is empty, set a dummy value of '#'
        char topElement = stack.empty() ? '#' : stack.pop();

        // If the mapping for this bracket doesn't match the stack's top element, return false.
        if (topElement != this.mappings.get(c)) {
          return false;
        }
      } else {
        // If it was an opening bracket, push to the stack.
        stack.push(c);
      }
    }

    // If the stack still contains elements, then it is an invalid expression.
    return stack.isEmpty();
  }
}
```
- 时间复杂度：O(n)，因为我们需要遍历给定字符串的每一个字符，并做push和pop操作会消耗掉O(1)次。
- 空间复杂度：O(n)，因为有种错误情况会导致我们push所有的左括号到整个栈，例如`(((((((`。

妈的！翻译得老子都开始怀疑人生了，估摸这是某个搞学术研究的家伙写的答案吧，睡觉！