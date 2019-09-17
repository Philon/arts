#include <vector>

using namespace std;

struct TreeNode {
  int val;
  TreeNode *left;
  TreeNode *right;
  TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};


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