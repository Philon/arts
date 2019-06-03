#include <vector>

using namespace std;

class Solution {
public:
  int trap(vector<int>& height) {
    int volume = 0;
    int left = 0;
    int right = height.size() - 1;
    int leftmax = 0, rightmax = 0;

    while (left < right) {
      if (height[left] < height[right]) {
        if (height[left] > leftmax) {
          leftmax = height[left];
        } else {
          volume += leftmax - height[left];
        }
        left++;
      } else {
        if (height[right] > rightmax) {
          rightmax = height[right];
        } else {
          volume += rightmax - height[right];
        }
        right--;
      }
    }
    

    return volume;
  }
};