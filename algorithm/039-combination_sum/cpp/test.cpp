#include "solution.h"
#include <assert.h>

void assertEqual(vector<vector<int>>& combination, vector<vector<int>>& expect) {
  assert(combination.size() == expect.size());

  for (auto a : combination) {
    for (int i = 0; i < expect.size(); i++) {
      auto b = expect[i];
      if (a.size() != b.size()) {
        continue;
      }

      for (int j = 0; j < a.size(); j++) {
        if (a[j] != b[j]) {
          goto next;
        }
      }
      expect.erase(expect.begin() + i);

next:
    }
  }
}

int main(int argc, char* argv[])
{
  vector<int> candidates = {2, 3, 5};
  Solution s;
  auto set = s.combinationSum(candidates, 8);
  vector<vector<int>> expect = {{2, 2, 2, 2}, {2, 3, 3}, {3, 5}};
  assertEqual(set, expect);
  return 0;
}