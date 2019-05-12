#include "solution.h"
#include <assert.h>

int main(int argc, char* argv[])
{
  Solution s;
  vector<int> nums = {4, -2, 2, 3, 1, 8, 12, 5};
  assert(s.firstMissingPositive(nums) == 6);
  return 0;
}