#include "solution.h"
#include <assert.h>

int main(int argc, char* argv[])
{
  Solution s;
  vector<int> nums = {2, 3, 1, 1, 4};
  assert(s.jump(nums) == 2);
  return 0;
}