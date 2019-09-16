#include <algorithm>

struct TreeNode {
  int val;
  TreeNode *left;
  TreeNode *right;
  TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};


class Solution {
public:
 int maxDepth(TreeNode* root) {
   return root ? std::max(maxDepth(root->left), maxDepth(root->right)) + 1 : 0;
 }
};