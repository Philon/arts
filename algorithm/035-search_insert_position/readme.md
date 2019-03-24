# [查找插入位置](https://leetcode.com/problems/search-insert-position/)

给一个一排序的数组和目标值，如果这个目标值能够找到的话返回它的索引。如果不能找到，返回它应该按照排序插入位置的索引。

你可以假设数组里没有重复。

```
Example 1:
Input: [1,3,5,6], 5
Output: 2

Example 2:
Input: [1,3,5,6], 2
Output: 1

Example 3:
Input: [1,3,5,6], 7
Output: 4

Example 4:
Input: [1,3,5,6], 0
Output: 0
```

## 我的思路：二分法

本题和第33、34题类似，都是在**已排序**的数组中查找目标值，二分法跑不掉的，就不多解释了。

不过在leetcode上有个比较搞笑的地方，由于测试用例太少，提交后的运行时间看人品，就不要太纠结beats了。

```c
int searchInsert(int* nums, int numsSize, int target) {
  int left = 0;
  int right = numsSize - 1;

  while (left <= right) {
    int i = (left + right) / 2;
    if (nums[i] > target) {
      right = i - 1;
    } else if (nums[i] < target) {
      left = i + 1;
    } else {
      return i;
    }
  }

  return left;
}
```
- 时间复杂度：O(logN)
- 空间复杂度：O(1)
