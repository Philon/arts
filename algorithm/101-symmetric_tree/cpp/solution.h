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
  //  if (root && root->left && root->right)
   return true;
 }

private:
  bool isSymetric(TreeNode* left, TreeNode* right) {
    if (left && right) {
      return isSymetric(left->left, right->right) && isSymetric(left->right, right->left);
    }
  }
};