# [路径之和](https://leetcode.com/problems/path-sum/)

给一个二叉树和一个数字sum，检查是否存在一条“从根到叶”的路径，其每个节点值之和等于sum。

注意：一个叶节点是没有子节点的。

```
Example:
给出以下一棵树，sum = 22
      5
     / \
    4   8
   /   / \
  11  13  4
 /  \      \
7    2      1
```
返回true，因为存在路径 5->4->11->2 的和等于22。

# 解题思路：递归法遍历每个叶节点，然后比较

题目要求求路径之和是否等于sum，其实可以反过来想，如果sum把整个路径减一遍，最终为0，即说明该路径之和等于sum。所以：

1. 用递归的方式遍历整棵树，每遍历一层就用sum减掉该层的子节点值。
2. 一旦发现自己是叶节点，检查sum减掉节点值后是否为零

```cpp
bool hasPathSum(TreeNode* root, int sum) {
    if (!root) {
      return false;
    } else if (!root->left && !root->right) {
      return !(sum - root->val);
    } else {
      sum -= root->val;
      return hasPathSum(root->left, sum) || hasPathSum(root->right, sum);
    }
  }
```

我的提交beats只有41%，本以为还有更牛逼的方案，结果看了答案后，貌似也是这个思路。难道是我的网速不好😄