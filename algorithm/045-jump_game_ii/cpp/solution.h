#include <vector>

using namespace std;

class Solution {
public:
  int jump(vector<int>& nums) {
    if (nums.size() < 2) {
      return 0;
    }

    int count = 0;
    int last = 0; // 表示能跳到的最后位置
    int current = 0; // 表示当前位置

    for (int i = 0; i < nums.size() - 1; i++) {
      last = max(last, nums[i] + i);
      if (i == current) {
        count++;
        current = last;
      }
    }

    return count;
  }
};