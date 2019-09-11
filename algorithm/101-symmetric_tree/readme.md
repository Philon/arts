# [对称树](https://leetcode.com/problems/symmetric-tree/)

给一个二叉树，检查它自己是否对称(即从中心对称)。

```
例如下面这个二叉树是对称的 [1,2,2,3,4,4,3] :

    1
   / \
  2   2
 / \ / \
3  4 4  3
 

但这颗树不对称 [1,2,2,null,3,null,3] :

    1
   / \
  2   2
   \   \
   3    3
 ```

注意： 如果你能用迭代或递归的方式做出来就更牛逼了。

## 解题思路：递归

其实这道题和《100-相同树》思想是一样的，都是通过递归，一层一层地遍历，关键就在于每一层该如何判断？

对于根节点，它是否对称取决于左右子节点是否相同，而再往下取决于“最左和最右”，“中间左和中间右”是否相同。用一个最典型的层来说——第三层——4个节点，如果它为镜像，那么应该满足“左左==右右 && 左右==右左”，注意这个相等的意思是要么都为空，要么不为空且值相同。

用代码实现就是这样：

```cpp
struct TreeNode {
  int val;
  TreeNode *left;
  TreeNode *right;
  TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};


class Solution {
public:
 bool isSymmetric(TreeNode* root) {
   return root ? isSymetric(root->left, root->right) : true;
 }

private:
  bool isSymetric(TreeNode* left, TreeNode* right) {
    if (left && right && left->val == right->val) {
      return isSymetric(left->left, right->right) && isSymetric(left->right, right->left);
    } else {
      return !left && !right;
    }
  }
};
```

- 时间复杂度： O(N)，N为二叉树的节点数量
- 空间复杂度： O(N)