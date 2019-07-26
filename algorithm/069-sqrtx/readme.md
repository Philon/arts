# [Sqrt(x)](https://leetcode.com/problems/sqrtx/)

实现`int sqrt(int x)`。

计算并返回x的平方根，x保证为非负整数。返回类型也是整数，小数部分直接忽略，仅保留整数部分。

```
Example 1:
Input: 4
Output: 2

Example 2:
Input: 8
Output: 2
Explanation: 8的平方根是2.82842...， 忽略小数部分，只返回2。
```

## 解题思路一、二分法

设 x = 22

1. x减半作为平方根root= (22 / 2) = 11
2. 计算root的平方，如果大于x，继续减一半的一半，否则就增加  
  11² = 121，显然大于22，所以root = 11 - (22 / 4) = 5.5
3. 计算5.5的平方：5.5² = 30.25大于22，继续减，root = 5.5 - (22 / 8) = 2.75
4. 2.75² = 7.5625小于22，所以增加，root = 2.75 + (22 / 16) = 4.125
5. 4.125² = 17.01... root = 4.125 + (22 / 32)

就这么循环，直到整数部分等于22。这便是通过二分法无限逼近平方根。当然，题目要求只取整数，所以不存在死循环的情况。

```cpp
int mySqrt(int x) {
  double root = (double)x / 2;
  unsigned long n = 0;
  unsigned long i = 2;
  do {
    n = root * root;
    i *= 2;
    if (n > x) {
      root -= (double)x / i;
    } else if (n < x) {
      root += (double)x / i;
    }
  } while (n != x);

  return (int)root;
}
```
- 时间复杂度：O(log n)，二分法不解释
- 空间复杂度：O(1)

## 解题思路二、牛顿迭代法

这是一种很牛逼的思路，代码实现不过两三行，但是你需要同时具备微积分、线性代数、几何、概率等多方面的数学基础。我尝试看了下，被繁杂的公式吓退了。

```cpp
int mySqrt(int x) {
  long r = x;
  while (r*r > x)
    r = (r + x/r) / 2;
  return r;
}
```