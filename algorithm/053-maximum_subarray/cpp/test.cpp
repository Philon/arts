#include "solution.h"
#include <assert.h>

int main(int argc, char* argv[])
{
  vector<int> nums = {-2,1,-3,4,-1,2,1,-5,4};
  assert((new Solution)->maxSubArray(nums) == 6);
  return 0;
}