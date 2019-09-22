# [二叉树的层次遍历 II](https://leetcode.com/problems/binary-tree-level-order-traversal-ii/)

给一个二叉树，自底向上遍历并返回每一层节点的值(从叶到根，一层层，从左到右)。

所谓的最大深度是指从二叉树的根节点开始向下找，离它最远的那个叶节点的数量。

```
Example:
给一个二叉树 [3,9,20,null,null,15,7],
    3
   / \
  9  20
    /  \
   15   7
返回自底向上的遍历结果为:
[
  [15,7],
  [9,20],
  [3]
]
```

## 解题思路：递归遍历

```cpp
class Solution {
public:
  vector<vector<int>> levelOrderBottom(TreeNode* root) {
    vector<vector<int>> levels;
    levelOrder(levels, 0, root);
    reverse(levels.begin(), levels.end());

    return levels;
  }

private:
  void levelOrder(vector<vector<int>>& results, int level, TreeNode* node) {
    if (!node) {
      return;
    }

    if (level >= results.size()) {
      results.push_back({});
    }

    results[level].push_back(node->val);
    levelOrder(results, level+1, node->left);
    levelOrder(results, level+1, node->right);
  }
};
```