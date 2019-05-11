#include <assert.h>
#include "solution.h"

void assertEqual(vector<vector<int>>& combinations,
                 vector<vector<int>>& expect) {
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

    next:
      continue;
    }
  }

  assert(expect.size() == 0);
}

int main(int argc, char* argv[]) {
  vector<int> candidates = {10, 1, 2, 7, 6, 1, 5};
  int target = 8;

  Solution s;
  auto combinations = s.combinationSum2(candidates, 8);
  vector<vector<int>> expect = {{1, 7}, {1, 2, 5}, {2, 6}, {1, 1, 6}};

  assertEqual(combinations, expect);
  return 0;
}