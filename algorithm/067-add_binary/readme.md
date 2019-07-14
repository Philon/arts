[二进制相加](https://leetcode.com/problems/add-binary/)

给两个二进制字符串，返回它们相加的结果（同样是二进制）。输入的字符串总是非空且仅包含1或0。

```
Example 1:
Input: a = "11", b = "1"
Output: "100"

Example 2:
Input: a = "1010", b = "1011"
Output: "10101"
```

## 解题思路

从两个数字的末尾(低位)开始相加，注意进位问题：
- n % 2可以得到当前位的值
- n / 2可以得到进位的值

```cpp
string addBinary(string a, string b) {
  string sum = "";
  int carry = 0;

  for (int i = a.size()-1, j = b.size()-1; i >= 0 || j >= 0; i--, j--) {
    int n = carry;
    if (i >= 0) n += a[i] - '0';
    if (j >= 0) n += b[j] - '0';
    sum += (n % 2) + '0';
    carry = n / 2;
  }

  if (carry) {
    sum += '1';
  }

  reverse(sum.begin(), sum.end());
  return sum;
}
```
- 时间复杂度：O(n)，取决于a和b之间较长的字符串
- 空间复杂度：O(n)，存储结果，max(a, b) + 1