# [爬楼梯](https://leetcode.com/problems/climbing-stairs/)

假设你正在爬楼梯，要爬n阶才能到达楼顶。每次你都可以爬1到2阶楼梯。有多少种爬到楼顶的方式呢？

注意：n总为正整数。

```
Example 1:
Input: 2
Output: 2
Explanation: 两种爬到楼顶的方法.
1. 1 step + 1 step
2. 2 steps

Example 2:
Input: 3
Output: 3
Explanation: 三种爬到楼顶的方法.
1. 1 step + 1 step + 1 step
2. 1 step + 2 steps
3. 2 steps + 1 step
```

## 解题思路：动态规划

我被这道题骗了。因为它属于easy，且看起来像排列组合，我一开始很自然地王数学阶乘之类方面去想，结果走不通。

本题比较容易理解的思路是动态规划，把每种方式都便利一遍，统计出那些能爬到楼顶的。(PS：我觉得但凡需要动态规划的算法都不能算easy😂)

当我们处于第i阶时，至少可以有两种方式爬到这里：
- 从第i-1往上爬一阶
- 从第i-2往上爬两阶

所以到达第i阶的方式总数应该为到达第i-1和i-2阶的方式总数之和。

// CPP实现
```cpp
class Solution {
public:
  int climbStairs(int n) {
    if (n == 1) {
      return 1;
    }

    int* dp = new int[n+1];
    dp[1] = 1;
    dp[2] = 2;
    for (int i = 3; i <= n; i++) {
      dp[i] = dp[i-1] + dp[i-2];
    }

    int count = dp[n];
    delete dp;
    return count;
  }
};
```
- 时间复杂度：O(n)，单次遍历
- 空间复杂度：O(n)，dp空间