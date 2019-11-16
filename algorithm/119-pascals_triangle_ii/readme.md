# [杨辉三角 II](https://leetcode.com/problems/pascals-triangle-ii/)

给一个无符号整数索引k，且k≤33，返回Pascal三角形(杨辉三角)的第k行。

注意：行的计数从0开始。

在Pascal三角形中，每个数字都等于它上边的两数之和。

```
Example:
Input: 3
Output: [1,3,3,1]
```

![](https://upload.wikimedia.org/wikipedia/commons/0/0d/PascalTriangleAnimated2.gif)

# 解题思路：

本题和118题类似

- 粗暴方式：生成杨辉三角，然后返回第k行
- 省内存方式：动态生成每一行杨辉三角，直到第k行，这样可以把空间复杂度降到n

我这里用的是第二种方法。

```cpp
vector<vector<int>> generate(int numRows) {
  vector<vector<int>> rows;

  for (int i = 0; i < numRows; i++) {
    vector<int> r(i+1, 1);
    for (int j = 1; j < i; j++) {
      r[j] = rows[i-1][j-1] + rows[i-1][j];
    }
    rows.push_back(r);
  }

  return rows;
}
```
- 时间复杂度：O(n²)，n表示rowIndex
- 空间复杂度：O(n)，n表示2 * rowIndex