# 第一个消失的正整数(https://leetcode.com/problems/first-missing-positive/)

41. First Missing Positive
给一个未排序的整型数组，找出最小的未在其中的正整数。

```
Example 1:
Input: [1,2,0]
Output: 3

Example 2:
Input: [3,4,-1,1]
Output: 2

Example 3:
Input: [7,8,9,11,12]
Output: 1
```

**注意**: 你的算法时间复杂度应该在O(n)内，空间复杂度为常数。

## 解题思路

首先抓住几个关键词：**未排序**、**最小正整数**、**时间O(n)**、**空间O(1)**。

要找出最小数字，无非就是给数字排序，或者用冒泡的方式，但题目把时间和空间复杂度都限定得很死，什么快速排序、哈希表就不要指望了。我看了下论坛上的思路，基本都是通过交换把所有值和下标绑定在一起，即`nums[4] == 4`，前提是`nums[i]`要小于数组长度且为正整数。如此一来，数组相当于做了部分升序，遍历这个部分排序的数组时，如果发现下标和元素值不想等时，下标就是我们要的答案。

```cpp
int firstMissingPositive(vector<int>& nums) {
  if (nums.empty()) {
    return 1;
  }

  int length = nums.size();
  for (int i = 0; i < length; i++) {
    while (nums[i] != i) {
      if (nums[i] > 0 && nums[i] < length && nums[i] != nums[nums[i]]) {
        swap(nums[i], nums[nums[i]]);
      } else {
        break;
      }
    }
  }

  for (int i = 1; i < length; i++) {
    if (nums[i] != i) {
      return i;
    }
  }

  return nums[0] == length ? length + 1 : length;
}
```
- 时间复杂度：O(n)，最坏情况下复杂度为3n，n为数组长度
- 空间复杂度：O(1)