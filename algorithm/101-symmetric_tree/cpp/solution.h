#include <stdio.h>

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