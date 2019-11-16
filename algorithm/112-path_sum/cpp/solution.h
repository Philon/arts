struct TreeNode {
  int val;
  TreeNode *left;
  TreeNode *right;
  TreeNode(int x) : val(x), left(0), right(0) {}
};

class Solution {
public:
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
};