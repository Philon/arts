#include <vector>

using namespace std;

class Solution {
 public:
  vector<vector<int>> combinationSum2(vector<int>& candidates, int target) {
    sort(candidates.begin(), candidates.end());
    vector<vector<int>> list;
    vector<int> combination;
    sum(candidates, target, list, combination, 0);

    return list;
  }

 private:
  void sum(vector<int>& candidates, int target, vector<vector<int>>& list,
           vector<int>& combination, int begin) {
    if (!target) {
      list.push_back(combination);
      return;
    }

    for (int i = begin; i < candidates.size() && target >= candidates[i]; i++) {
      if (i == begin || candidates[i] != candidates[i - 1]) {
        combination.push_back(candidates[i]);
        sum(candidates, target - candidates[i], list, combination, i + 1);
        combination.pop_back();
      }
    }
  }
};