# [转化有序数组为二叉搜索树](https://leetcode.com/problems/convert-sorted-array-to-binary-search-tree)

给一个升序元素组成的数组，将其转换为高度平衡的二叉树。

所谓高度平衡二叉树是指：两个子树的每个节点深度相差不超过1。

```
Example:
给一个有序数组: [-10,-3,0,5,9],
其中一种可能的答案为: [0,-3,9,-10,null,5], 即高度平衡的二叉树:
      0
     / \
   -3   9
   /   /
 -10  5
```

# 解题思路：递归二分法

其实这道题只需要看官方给的这个例子就有个大体思路，数组[-10,-3,0,5,9]生成二叉树后，根节点是0，左右分别是-3和9，说明它是从最中间开始，往左右两边延展开来。

如果要遍历整个数组，又要确保均衡地分散在每一层中，用递归+二分法在合适不过，每次递归都会生层一层，每一层的节点都可以通过二分法获得左右节点。

```cpp
class Solution {
public:
  TreeNode* sortedArrayToBST(vector<int>& nums) {
    return bst(nums, 0, nums.size());
  }
private:
  TreeNode* bst(vector<int>& nums, int left, int right) {
    if (left >= right) {
      return NULL;
    }

    int middle = (right + left) / 2;
    TreeNode* node = new TreeNode(nums[middle]);
    node->left = bst(nums, left, middle);
    node->right = bst(nums, middle+1, right);

    return node;
  }
};
```