#include "solution.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void assertSolution(const char* s, int target, int expected) {
  int numsSize = strlen(s);
  int* nums = calloc(numsSize, sizeof(char));
  for (int i = 0; i < numsSize; i++) {
    nums[i] = s[i] - '0';
  }

  assert(search(nums, numsSize, target) == expected);
}

int main(int argc, char* argv[])
{
  assertSolution("4567123", 5, 1);
  assertSolution("12", 3, -1);
  assertSolution("", 5, -1);
  return 0;
}