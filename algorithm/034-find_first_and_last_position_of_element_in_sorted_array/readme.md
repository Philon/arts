# [在排序数组中找出元素的首尾位置]

给定一个由升序排列的整型数组`nums`，找出给定目标值`target`在其中开始和结束的位置。

你的算法时间复杂度必须在O(log n)内。

如果目标值在数组中找不到，返回`[-1, -1]`。

```
Example 1:
Input: nums = [5,7,7,8,8,10], target = 8
Output: [3,4]

Example 2:
Input: nums = [5,7,7,8,8,10], target = 6
Output: [-1,-1]
```

由于此题其实和第33区别不大，个人思路就不写了，直接翻译官方的。

## 方案一、直线扫描

### 概述

检查每一个索引是否是`target`，这必然会扫描得更多。

### 算法

首先，我们从左开始直线扫描`nums`，直到找出`target`后break。如果一直没找到说明不存在`target`，就返回错误结果或`[-1,-1]`。为了找出有效的左边索引，我们可以在第二次的时候直线扫面，但这次从右边开始。因为左边的之前已经找到了，从右边开始能节约时间。然后简单地返回包含这两个索引的列表。

```java
class Solution {
    public int[] searchRange(int[] nums, int target) {
        int[] targetRange = {-1, -1};

        // find the index of the leftmost appearance of `target`.
        for (int i = 0; i < nums.length; i++) {
            if (nums[i] == target) {
                targetRange[0] = i;
                break;
            }
        }

        // if the last loop did not find any index, then there is no valid range
        // and we return [-1, -1].
        if (targetRange[0] == -1) {
            return targetRange;
        }

        // find the index of the rightmost appearance of `target` (by reverse
        // iteration). it is guaranteed to appear.
        for (int j = nums.length-1; j >= 0; j--) {
            if (nums[j] == target) {
                targetRange[1] = j;
                break;
            }
        }

        return targetRange;
    }
}
```

- 时间复杂度：O(n)
- 空间复杂度：O(1)

## 方案二、二分法

### 概述

由于数组是一个已排序的，我们可以采用二分法来定位左右位置。

### 算法

整个算法的运行方式和线性扫描的方案是很相似的，都是从最左右两端开始执行。只是从这里开始采用二分法来查询数组，只需要小范围的调整。首先，由于我们要定位最左(或最右)索引中是否包含`target`，一旦找到目标值算法就终止。然后，我们开始继续通过搜索`lo==hi`来包含掉与目标值相同的索引。

(剩下关于二分法的工作机制就不介绍了)。

```java
class Solution {
    // returns leftmost (or rightmost) index at which `target` should be
    // inserted in sorted array `nums` via binary search.
    private int extremeInsertionIndex(int[] nums, int target, boolean left) {
        int lo = 0;
        int hi = nums.length;

        while (lo < hi) {
            int mid = (lo + hi) / 2;
            if (nums[mid] > target || (left && target == nums[mid])) {
                hi = mid;
            }
            else {
                lo = mid+1;
            }
        }

        return lo;
    }

    public int[] searchRange(int[] nums, int target) {
        int[] targetRange = {-1, -1};

        int leftIdx = extremeInsertionIndex(nums, target, true);

        // assert that `leftIdx` is within the array bounds and that `target`
        // is actually in `nums`.
        if (leftIdx == nums.length || nums[leftIdx] != target) {
            return targetRange;
        }

        targetRange[0] = leftIdx;
        targetRange[1] = extremeInsertionIndex(nums, target, false)-1;

        return targetRange;
    }
}
```

- 时间复杂度：O(log n)，二分法，不解释
- 空间复杂度：O(1)