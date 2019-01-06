# [4数之和](https://leetcode.com/problems/4sum/)

给定一个整型数字构成的数组以及一个目标值，这些找出所有符合`a + b + c + d = target`的四个元素组合。

**注意**：  
返回的答案当中可不能有重复的哟！

```
Example:

给定数组nums = [1, 0, -1, 0, -2, 2], 目标值 target = 0.

所有符合的答案集合为:
[
  [-1,  0, 0, 1],
  [-2, -1, 1, 2],
  [-2,  0, 0, 2]
]
```

## 我的思路、双指针

这道题目和[3Sum](../015-3sum/)其实没什么大的区别，无非就是多找一个元素而已，所以思路也是一样的。对于3sum的思路大体而言就是摁住开头的一个，剩下两个通过双指针遍历，所以4个数字组合的玩法就是，摁住前两个，剩下两个双指针遍历。

与3sum解题几乎相同的步骤：
1. 将输入数组nums做升序处理`qsort`。
2. 两层for循环嵌套，摁住前两个数
3. 第三层嵌套双指针while循环，遍历后两个数
4. 每次找到符合条件的四个元素组合后，务必注意`去重`

可优化空间：
1. 由于nums已经做过升序处理，在进入每一层for循环前，可以检查`起始之和——最小值`是否都大于目标值，若为真，直接进入下一次。
2. 同理，检查`末尾之和——最大值`是否都小于目标值，若为真，之前中断循环。

另外，貌似还有更好的优化方式，我没有深入研究了，目前只能做到60%的beast。

OK，这题也没有官方solution。
```c
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
```