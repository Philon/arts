#include "solution.h"
#include <assert.h>

int main(int argc, char* argv[])
{
  vector<int> nums1 = {1,2,3,0,0,0};
  vector<int> nums2 = {2,5,6};
  Solution s;
  s.merge(nums1, 3, nums2, nums2.size());
  assert(nums1 == vector<int>({1,2,2,3,5,6}));
  return 0;
}