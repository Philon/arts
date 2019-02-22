#include <stdlib.h>

void nextPermutation(int* nums, int numsSize) {
  int i = numsSize - 2;
  for ( ; i >= 0; i--) {
    if (nums[i] < nums[i + 1]) break;
  }

  for (int j = numsSize-1; i >= 0 && j >= 0; j--) {
    if (nums[i] < nums[j]) {
      int tmp = nums[i];
      nums[i] = nums[j];
      nums[j] = tmp;
      break;
    }
  }

  for (int l = i+1, r = numsSize-1; l < r; l++, r--) {
    int tmp = nums[l];
    nums[l] = nums[r];
    nums[r] = tmp;
  }
}
