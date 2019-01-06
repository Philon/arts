#include <stdlib.h>

int compartion(const void* a, const void* b) {
  return *(int*)a - *(int*)b;
}

int** fourSum(int* nums, int numsSize, int target, int* returnSize) {
  qsort(nums, numsSize, sizeof(int), compartion);

  int e1 = nums[numsSize - 1];
  int e2 = nums[numsSize - 2];
  int e3 = nums[numsSize - 3];
  int** results = (int**)calloc(numsSize * numsSize * numsSize, sizeof(int*));
  int count = 0;
  for (int i = 0; i < numsSize - 3; i++) {
    // 检查如果存在从“源头”就不满足条件的情况，就不必继续遍历了
    if (nums[i] + nums[i+1] + nums[i+2] + nums[i+3] > target) break;
    else if (nums[i] + e3 + e2 + e1 < target) continue;

    for (int j = i + 1; j < numsSize - 2; j++) {
      if (nums[i] + nums[j] + nums[j+1] + nums[j+2] > target) break;
      else if (nums[i] + nums[j] + e2 + e1 < target) continue;
      
      // 双指针遍历数组
      int left = j + 1;
      int right = numsSize - 1;
      while (left < right) {
        int sum = nums[i] + nums[j] + nums[left] + nums[right];
        if (sum > target) {
          right--;
        } else if (sum < target) {
          left++;
        } else {
          int* quadruplets = (int*)calloc(4, sizeof(int));
          quadruplets[0] = nums[i];
          quadruplets[1] = nums[j];
          quadruplets[2] = nums[left];
          quadruplets[3] = nums[right];
          results[count] = quadruplets;
          // 检查重复答案
          while (i + 1 < numsSize && nums[i] == nums[i+1]) i++;
          while (j + 1 < numsSize && nums[j] == nums[j+1]) j++;
          while (left + 1 < right && nums[left] == nums[left+1]) left++;
          while (left < right - 1 && nums[right] == nums[right-1]) right--;
          left++, right--, count++;
        }
      }
    }
  }

  *returnSize = count;
  return results;
}