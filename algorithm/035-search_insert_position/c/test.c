#include "solution.h"
#include <assert.h>

void assertSolution(const char* s, int target, int expected) {
  int numsSize = strlen(s);
  int* nums = calloc(numsSize, sizeof(char));
  for (int i = 0; i < numsSize; i++) {
    nums[i] = s[i] - '0';
  }

  assert(searchInsert(nums, numsSize, target) == expected);
}

int main(int argc, char* argv[])
{
  assertSolution("1345678", 0, 0);
  assertSolution("12345", 6, 5);
  assertSolution("1", 0, 0);
  assertSolution("", 0, 0);
  return 0;
}