#include "solution.h"
#include <assert.h>

void assertEqual(vector<vector<int>>& a, vector<vector<int>>& b) {
  for (int i = 0; i < a.size(); i++) {
    for (int j = 0; j < b.size(); j++) {
      if (a[i] == b[j]) {
        a.erase(a.begin() + i);
        b.erase(b.begin() + j);
        i--;
        break;
      }
    }
  }

  assert(a.size() == 0 && b.size() == 0);
}

int main(int argc, char* argv[]) {
  vector<int> nums = {1, 2, 3,};
  vector<vector<int>> permutations = {{1, 2, 3}, {1, 3, 2}, {2, 1, 3}, {2, 3, 1}, {3, 1, 2}, {3, 2, 1}};
  auto result = (new Solution())->permute(nums);
  assertEqual(result, permutations);
  return 0;
}