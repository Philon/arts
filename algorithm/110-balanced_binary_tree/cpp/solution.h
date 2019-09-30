#include <iostream>

using namespace std;

struct TreeNode {
  int val;
  TreeNode *left;
  TreeNode *right;
  TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

class Solution {
public:
  bool isBalanced(TreeNode* root) {
    return root ? isBalanced(root->left) && isBalanced(root->right) && abs(depth(root->left)-depth(root->right)) < 2 : true;
  }

  int depth(TreeNode* node) {
    return node ? max(depth(node->left), depth(node->right))+1 : 0;
  }
};