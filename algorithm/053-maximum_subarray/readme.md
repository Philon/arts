# [最大子数组](https://leetcode.com/problems/maximum-subarray/)

给一个整数数字`nums`，找出其和值最大的子数组(至少包含一个数字)，并返回它们的和。

```
Example:

Input: [-2,1,-3,4,-1,2,1,-5,4],
Output: 6
解释: 因为其中的[4,-1,2,1]的和等于6，最大。
```

进阶：  
如果你找到了O(n)的解题方案，尝试一下分治法，这个更精妙。

## 解题思路

本题说是找出子数组，其实是找数组之和，换句话说，可以通过遍历来求数组之和。然后，**如果当前元素值已经比当前的和还大，是不是说明可以放弃之前的子数组了**。最后，题目要求返回的是最大和的值，所以明确可以通过冒泡法来获取最大值。

```cpp
class Solution {
public:
  int maxSubArray(vector<int>& nums) {
    int largest = nums[0];
    int sum = largest;

    for (int i = 1; i < nums.size(); i++) {
      sum = max(nums[i], sum + nums[i]);
      largest = max(largest, sum);
    }

    return largest;
  }
};
```
- 时间复杂度：O(n)，n取决于数组长度
- 空间复杂度：O(1)

~PS: 我没有进一步了解分治法~