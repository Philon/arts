# [数独解答](https://leetcode.com/problems/sudoku-solver/)

写一个程序以填满栅格的方式解谜数独题目。

一个数独解答必须满足以下全部规则：
- 每一行栅格中必须出现数字`1-9`的每一种
- 每一列栅格中必须出现数字`1-9`的每一种
- 每个九宫格的`3x3`栅格中必须出现数字`1-9`的每一种

空白的栅格将会以`.`字符取代

![](https://upload.wikimedia.org/wikipedia/commons/thumb/f/ff/Sudoku-by-L2G-20050714.svg/250px-Sudoku-by-L2G-20050714.svg.png)
这是一个数据残局

![](https://upload.wikimedia.org/wikipedia/commons/thumb/3/31/Sudoku-by-L2G-20050714_solution.svg/250px-Sudoku-by-L2G-20050714_solution.svg.png)
标记红色是解谜后的数字

注意：
- 提供的数独板仅包含数字`1-9`和字符`.`
- 你可以假设提供的数独谜题仅有唯一解
- 提供的数独板总是9x9的大小

## 我的思路：回溯法

和上一题一样，对于这种谜题游戏我基本没有什么思路，都是看论坛里，主流的思路是回溯法，只是在运算和优化上有所区别。

这道题用回溯法解决，可以概括为一个字——猜！也就是一个空格从1-9顺着猜，总能蒙对的。所以大体思路如下：
1. 二维数组中遇到1-9字符的全部跳过
2. 遇到`'.'`字符是，顺着填入`1-9`，并检查行、列、子块中是否有冲突
3. 如果没有冲突，接着找下一个`'.'`，重复步骤2
4. 如果发现冲突了(无解了)，说明是前面的数字不对，顺着递归回去把之前填过的数字全部擦写为`'.'`，重复步骤2
5. 直到9x9栅格全部遍历完

这种方法简单易懂，几乎把每种答案都猜个遍，所以时间复杂度是极高的，而且递归调用对栈区的开销也不小。  
可惜，我也想不出更牛逼的解决方案了。

```c
bool check(char** board, int i, int j, char c) {
  for (int k = 0; k < 9; k++) {
    if (board[i][k] == c) return false;
    if (board[k][j] == c) return false;
    if (board[i-i%3+k/3][j-j%3+k%3] == c) return false;
  }

  return true;
}

bool solve(char** board, int row, int col) {
  if (col == 9) {
    row++;
    return row == 9 ? true : solve(board, row, 0);
  }

  if (board[row][col] != '.') {
    return solve(board, row, col+1);
  }

  for (char c = '1'; c <= '9'; c++) {
    if (check(board, row, col, c)) {
      board[row][col] = c;
      if (solve(board, row, col+1)) return true;
      board[row][col] = '.';
    }
  }

  return false;
}

void solveSudoku(char** board, int boardRowSize, int boardColSize) {
  solve(board, 0, 0);
}
```
- 时间复杂度：O(9^n*n)，n是空白格子数
- 空间复杂度：O(n)，n是空白格子数，虽然没有分配内存，但递归调用，参数压栈是动态的