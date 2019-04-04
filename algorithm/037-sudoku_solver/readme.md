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

