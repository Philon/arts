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