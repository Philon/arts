# [两数相除](https://leetcode.com/problems/divide-two-integers/)

给两个整型数字，`dividend`被除数和`divisor`除数，在不适用乘法、除法、取模等运算符的前提下将两数相除。返回其商。

```
Example 1:
Input: dividend = 10, divisor = 3
Output: 3

Example 2:
Input: dividend = 7, divisor = -3
Output: -2
```

**注意**：  
除数和被除数都是32位有符号整型。  
除数永不为0.  
假设我们的运行环境仅能存放32位的整型，其值范围:[-2的31次方, 2的31次方 - 1]。为了本题的目的，假定你的函数返回2的31次方-1时表示除法结果溢出。

---
我之前知道，可以通过位运算实现加法，进而通过符号取反可以实现减法，通过累加实现乘法，循环减实现除法。如果按这个思路需要实现的东西会非常多，仔细读题。我们不能使用的运算符仅仅是`'*' '/' '%'`三个，所以`'+' '-'`两个符号是可以用的。


## 我的思路：循环减

这种方法最简单，但也最慢。

1. 获取除数和被除数的符号——用于返回值的符号
2. 将除数和被除数各去绝对值——便于循环减
3. 循环用被除数去减除数，每减一次，商加1，直到被除数彻底小于除数
4. 根据第一步求得的符号，返回涵正负号的商值

```c
#define INTMAX 2147483647
#define INTMIN -2147483648

int divide(int dividend, int divisor) {
  if ((dividend == INTMIN && divisor == -1) || divisor == 0) {
    return INTMAX;
  }

  int sign = (dividend < 0) ^ (divisor < 0);
  int quotient = 0;
  long int dn = dividend < 0 ? -(long int)dividend : dividend;
  long int dr = divisor < 0 ? -(long int)divisor : divisor;

  while (dn >= dr) {
    dn -= dr;
    quotient++;
  }

  return sign ? -quotient : quotient;
}
```
- 时间复杂度：O(n)， n相当于dividend/divisor，最坏情况下会执行INTMIN次
- 空间复杂度：O(1)， 没有分配什么

## 我的思路：优化循环减

上面的方法中，如果**被除数远大于除数**的情况下，效率是极低的，所以要从这种情况入手来优化。

原理不难理解，每次只减除数的“一倍”太慢了，那为什么不一次性减“很多倍”呢？

我们都知道，1左移一位是2，再左移是4，都是2倍的指数增长。因此反过来想，一个数A右移n位，还B大，那么说明A至少是B的n倍。

```c
int divide(int dividend, int divisor) {
  // 排除根本不可能的除法运算
  if ((dividend == INTMIN && divisor == -1) || divisor == 0) {
    return INTMAX;
  }
  
  // 为了防止INTMAX和INTMIN的溢出问题，需要用long int来存储
  long int dn = dividend < 0 ? -(long int)dividend : dividend;
  long int dr = divisor < 0 ? -(long int)divisor : divisor;
  int quotient = 0;

  for (int i = 31; i >= 0; i--) {
    // 如果移位后还大于除数，说明被除数至少比它大 1 << i倍
    if ((dn >> i) >= dr) {
      quotient += 1 << i;
      dn -= dr << i;
    }
  }

  // 根据除数和被除数确定商的符号，并返回
  return (dividend < 0) ^ (divisor < 0) ? -quotient : quotient;
}
```
- 时间复杂度：O(1)， 固定32次
- 空间复杂度：O(1)