#include "solution.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void assertSolution(const char* s, int target, const char* expected) {
  int numsSize = strlen(s);
  int* nums = calloc(numsSize, sizeof(int));
  for (int i = 0; i < numsSize; i++) {
    nums[i] = s[i] - '0';
  }

  int returnSize = 0;
  int* result = searchRange(nums, numsSize, target, &returnSize);
  for (int i = 0; i < returnSize; i++) {
    assert(result[i] == (expected[i] - '0'));
  }
}

int main(int argc, char* argv[])
{
  assertSolution("1122334456778", 4, "67");
  assertSolution("12345", 3, "22");
  assertSolution("22", 2, "01");
  return 0;
}