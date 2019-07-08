#include <vector>

using namespace std;

class Solution {
public:
  int maxSubArray(vector<int>& nums) {
    int largest = nums[0];
    int sum = largest;

    for (int i = 1; i < nums.size(); i++) {
      sum = max(nums[i], sum + nums[i]);
      largest = max(largest, sum);
    }

    return largest;
  }
};