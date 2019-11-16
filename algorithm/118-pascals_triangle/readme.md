# [杨辉三角](https://leetcode.com/problems/pascals-triangle/)

给一个无符号整数numRows，生成一个Pascal三角形(杨辉三角)。在Pascal三角形中，每个数字都等于它上边的两数之和。

```
Example:
Input: 5
Output:
[
     [1],
    [1,1],
   [1,2,1],
  [1,3,3,1],
 [1,4,6,4,1]
]
```

![](https://upload.wikimedia.org/wikipedia/commons/0/0d/PascalTriangleAnimated2.gif)

# 解题思路：

没什么技巧，就是根据题意，一层层地计算当前值，然后push到集合中即可

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
- 时间复杂度：O(n²)，n是numRows
- 空间复杂度：O(n²)