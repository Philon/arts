#include <vector>

using namespace std;

class Solution {
public:
  void merge(vector<int>& nums1, int m, vector<int>& nums2, int n) {
    for (m -= 1, n -= 1; m >= 0 && n >= 0; ) {
      nums1[m+n+2] = nums1[m] > nums2[n] ? nums1[m--] : nums2[n--];
    }
    while (n >= 0) {
      nums1[m+n+2] = nums2[n--];
    }
  }
};