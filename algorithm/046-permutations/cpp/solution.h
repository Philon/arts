#include <vector>

using namespace std;

class Solution {
public:
  vector<vector<int>> permute(vector<int>& nums) {
    vector<vector<int>> permutations;
    permute(nums, permutations, 0);
    return permutations;
  }

private:
  void permute(vector<int>& nums, vector<vector<int>>& permutations, int i) {
    if (i == nums.size()) {
      permutations.push_back(nums);
    }

    for (int j = i; j < nums.size(); j++) {
      swap(nums[i], nums[j]);
      permute(nums, permutations, i + 1);
      swap(nums[i], nums[j]);
    }
  }
};