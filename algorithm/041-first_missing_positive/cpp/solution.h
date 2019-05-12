#include <vector>

using namespace std;

class Solution {
public:
  int firstMissingPositive(vector<int>& nums) {
    if (nums.empty()) {
      return 1;
    }

    int length = nums.size();
    for (int i = 0; i < length; i++) {
      while (nums[i] != i) {
        if (nums[i] > 0 && nums[i] < length && nums[i] != nums[nums[i]]) {
          swap(nums[i], nums[nums[i]]);
        } else {
          break;
        }
      }
    }

    for (int i = 1; i < length; i++) {
      if (nums[i] != i) {
        return i;
      }
    }

    return nums[0] == length ? length + 1 : length;
  }
};