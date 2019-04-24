#include <vector>

using namespace std;

class Solution {
public:
  vector<vector<int>> combinationSum(vector<int>& candidates, int target) {
    sort(candidates.begin(), candidates.end());
    vector<vector<int>> list;
    vector<int> combination;
    sum(candidates, target, list, combination, 0);
  }

private:
  void sum(vector<int>& candidates, int target, vector<vector<int>>& list, vector<int>& combination, int begin) {
    if (target <= 0) {
      list.push_back(combination);
      return;
    }

    for (int i = begin; i != candidates.size() && target >= candidates[i]; i++) {
      combination.push_back(i);
      sum(candidates, target - candidates[i], list, combination, i);
      combination.pop_back();
    }
  }
};