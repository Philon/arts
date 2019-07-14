# [加1](https://leetcode.com/problems/plus-one/)

给一个非空数组，相当于一个非负数，给这个整数加一。

你可以假定这个数字不会以0开头，除非它本身就是0。

```
Example 1:
Input: [1,2,3]
Output: [1,2,4]
Explanation: 相当于数字123+1。

Example 2:
Input: [4,3,2,1]
Output: [4,3,2,2]
Explanation: 相当于数组4322+1。
```

## 解题思路

整数+1，唯一要注意的地方就是进位，至于算法方面自然是从整数的最低位开始遍历，即数组的最后一个元素。

```cpp
vector<int> plusOne(vector<int>& digits) {
  int carry = 1;

  for (int i = digits.size() - 1; i >= 0 && carry > 0; i--) {
    digits[i] += carry;
    carry = digits[i] / 10;
    digits[i] = digits[i] % 10;
  }

  if (carry > 0) {
    digits.insert(digits.begin(), carry);
  }

  return digits;
}
```
- 时间复杂度：O(n)，n取决于数组长度或是否存在进位
- 空间复杂度：O(1)