#include <stdio.h>
#include "solution.h"
#include <assert.h>
#include <stdarg.h>


void assertSolution(int* nums, int numsSize, int target, int expected[][4]) {
  int count = 0;
  int** results = fourSum(nums, numsSize, target, &count);

  for (int i = 0; i < count; i++) {
    assert(expected[i] != 0);
    for (int j = 0; j < 4; j++) {
      assert(results[i][j] == expected[i][j]);
    }
    free(results[i]);
  }
  free(results);
}

int main(int argc, char* argv[])
{
  int nums[] = {-5,-2,-4,-2,-5,-4,0,0};
  int expected[][4] = {
    {-5, -4, -4, 0},
    {-5, -4, -2, -2},
  };

  assertSolution(nums, sizeof(nums) / sizeof(int), -13, expected);
  return 0;
}