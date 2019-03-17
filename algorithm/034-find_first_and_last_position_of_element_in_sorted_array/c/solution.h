#include <stdlib.h>

void binsearch(int* nums, int left, int right, int target, int* results) {
  if (right - left < 2) {
    if (target == nums[left]) {
      results[0] = left;
      results[1] = target == nums[right] ? right : left;
    } else if (target == nums[right]) {
      results[0] = target == nums[left] ? left : right;
      results[1] = right;
    } else {
      results[0] = results[1] = -1;
    }
    return;
  }

  int mid = (left + right) / 2;
  if (target > nums[mid]) {
    binsearch(nums, mid, right, target, results);
  } else if (target < nums[mid]) {
    binsearch(nums, left, mid, target, results);
  } else {
    int i = mid;
    while (i > left && nums[i-1] == target) i--;
    results[0] = i;
    while (i < right && nums[i+1] == target) i++;
    results[1] = i;
  }
}

int* searchRange(int* nums, int numsSize, int target, int* returnSize) {
  int* results = (int*)calloc(2, sizeof(int));
  if (numsSize > 0) {
    binsearch(nums, 0, numsSize-1, target, results);
  } else {
    results[0] = results[1] = -1;
  }

  *returnSize = 2;
  return results;
}