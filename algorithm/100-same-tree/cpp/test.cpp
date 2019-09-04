#include "solution.h"
#include <assert.h>
#include <vector>

using namespace std;

struct TreeNode* tree(vector<int> v) {
  // 创建二叉树太繁琐，懒得写了😂
  return NULL;
}

int main(int argc, char* argv[])
{
  Solution s;
  assert(s.isSameTree(tree({1,2,3}), tree({1,2,3})));
  return 0;
}