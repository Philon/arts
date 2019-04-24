#include "solution.h"
#include <assert.h>

void assertEqual(vector<vector<int>>& combinations, vector<vector<int>>& expect) {
  assert(combinations.size() == expect.size());

  for (auto& a : combinations) {
    for (int i = 0; i < expect.size(); i++) {
      auto& b = expect[i];
      if (a.size() != b.size()) {
        continue;
      }
      
      for (int j = 0; j < a.size(); j++) {
        if (a[j] != b[j]) {
          goto next;
        }
      }

      expect.erase(expect.begin() + i);
      break;
      
next: continue;
    }
  }

  assert(expect.size() == 0);
}

int main(int argc, char* argv[])
{
  Solution s;
  vector<int> candidates = {2, 3, 5};
  auto set = s.combinationSum(candidates, 8);
  vector<vector<int>> expect = {{2, 2, 2, 2}, {2, 3, 3}, {3, 5}};
  assertEqual(set, expect);

  return 0;
}