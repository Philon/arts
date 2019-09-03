# [相同的树](https://leetcode.com/problems/same-tree/)

给两个二叉树，写个函数用于检查它们是否相同。

如果二叉树的结构和值都相等，那可以认为它们是相同的。

```
Example 1:
Input:     1         1            Output: true
          / \       / \
         2   3     2   3

        [1,2,3],   [1,2,3]

Example 2:
Input:     1         1            Output: false
          /           \
         2             2

        [1,2],     [1,null,2]

Example 3:
Input:     1         1            Output: false
          / \       / \
         2   1     1   2

        [1,2,1],   [1,1,2]
```