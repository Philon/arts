# 二叉树的最小深度(https://leetcode.com/problems/minimum-depth-of-binary-tree/)

给一个二叉树，找出它的最小深度。所谓最小深度是指从根节点到叶节点的最短路径的节点数。

注意：叶节点是没有孩子的节点。

```
Example:

二叉树 [3,9,20,null,null,15,7],
    3
   / \
  9  20
    /  \
   15   7
它的最小深度 = 2.
```

## 解题思路：参考[104二叉树最大深度](https://github.com/Philon/arts/tree/master/algorithm/104-maximum_depth_of_binary_tree)

思路可概括为递归+冒泡法：

1. 通过递归的方式遍历树的每一个节点
2. 当遍历到叶节点时开始返回，每返回一次深度+1
3. 通过冒泡的方式计算每个子节点“左”和“右”的最短路径

但是务必注意题目说明“叶节点没有孩子”，比如下面这个测试用例：

```
   1
  / \
 2
二叉树的最小深度为：2
```

如果单纯地用冒泡方式比较左右子树的深度，很容易犯错：左树深度为2，右树深度为1，所以返回1。

```cpp
class Solution {
public:
  int minDepth(TreeNode* root) {
    if (!root) {
      return 0;
    } else if (root->left && root->right) {
      return min(minDepth(root->left), minDepth(root->right)) + 1;
    } else {
      return (root->left ? minDepth(root->left) : minDepth(root->right)) + 1;
    }
  }
};
```

最快运行4ms，beats 100%