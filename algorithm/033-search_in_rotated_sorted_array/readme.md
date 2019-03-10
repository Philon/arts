# 在旋转排序数组中查找

设：一个通过升序排序的数组被旋转了，转轴是你事先不知道的。

(比如：[0,1,2,4,5,6,7]可能变成了[4,5,6,7,0,1,2])。

会给你一个目标值用于搜索，如果能够在数组中找到，就返回它的索引，否则就返回-1。

你可以假设数组中没有重复元素。

你的算法时间复杂度必须在O(log n)以内。

```
Example 1:
Input: nums = [4,5,6,7,0,1,2], target = 0
Output: 4

Example 2:
Input: nums = [4,5,6,7,0,1,2], target = 3
Output: -1
```

## 我的思路

本题的题目可能比较绕，其实就一句话：**找出数组中指定元素的索引，如果没有就返回-1**。

所以有一个非常简单的答案😂：
```c
int search(int* nums, int numsSize, int target) {
  for (int i = 0; i < numsSize; i++) {
    if (nums[i] == target) return i;
  }

  return -1;
}
```

不过题目最后明确要求，时间复杂度在O(log n)以内，而上边代码的时间复杂度明显是n，太慢了！

如果是一个排序过的数组，要降低时间复杂度，用二分法查找再合适不过，可这道题难就难在输入的数组不仅排序过，还被旋转过。因此单纯的二分查找会让你最终找不着北。

### 逆二分法

我不知道该如何形容这个算法，所以就取这么个名字，大体思路是这样的：

在一个排序过的数组中，二分查找的核心逻辑是：**确定目标值所在范围**，即通过与数组中心元素比较大小，预测其是在左边还是右边。由于本题的数组是被“旋转”过的，比如`[5, 6, 7, 10, 11, 1, 3, 4]`，这个数组的中心是10，假设要找的目标是3，如果用传统的二分查找，由于3小于10，算法自然会往左边去找——肯定找不到。

在仔细看这个数组，以10为中心，可以确定`5-10`之间的元素是升序，`10-4`之间的元素是乱序，换句话说，**不论这个数组怎么旋转，肯定有一半依然是排序过的**！确认排序过的那一半在左边还是右边非常容易，即左界值小于中心，肯定是左半部分；右界值大于中心，肯定是右半部分。有了这半个升序数组，就可以轻易地确定目标值是否在这个范围内了。

此时，反过来想象：**二分法是确定目标值所在范围，逆二分法就是确定目标值不在哪个范围**。说白了，如果能明确目标值不在此范围，那么它可能就在另一半部分。

所以算法实现如下，递归+二分查找：
```c
int binarySearch(int* nums, int left, int right, int target) {
  int mid = (right + left) / 2;
  if (nums[mid] == target) {
    return mid;
  } else if (nums[left] == target) {
    return left;
  } else if (nums[right] == target) {
    return right;
  }

  // left等于mid，说明已经找到头了
  if (left == mid) {
    return -1;
  }

  if (nums[mid] > nums[left]) {
    // 左半部分是升序，根据其值范围，确定目标值在左还是右
    return target > nums[left] && target < nums[mid]
               ? binarySearch(nums, left, mid, target)
               : binarySearch(nums, mid, right, target);
  } else {
    // 右半部分是升序，根据其值范围，确定目标值在左还是右
    return target > nums[mid] && target < nums[right]
               ? binarySearch(nums, mid, right, target)
               : binarySearch(nums, left, mid, target);
  }
}

int search(int* nums, int numsSize, int target) {
  if (nums == NULL || numsSize == 0) {
    return -1;
  } else {
    return binarySearch(nums, 0, numsSize - 1, target);
  }
}
```
- 时间复杂度：O(log n)，二分法，不解释
- 空间复杂度：O(1)，除了基本变量，没有分配内存空间
