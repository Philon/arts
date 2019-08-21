# [合并升序数组](https://leetcode.com/problems/merge-sorted-array/)

给两个排过序的整型数组`nums1`和`nums2`，将nums2合并到nums1中为一个数组。

注意：

- nums1和nums2都各自初始化m和n个数字
- 你可以假设nums1有足够的空间(即长度大于或等于m+n)来存储nums2的元素

```
Example:

Input:
nums1 = [1,2,3,0,0,0], m = 3
nums2 = [2,5,6],       n = 3
Output: [1,2,2,3,5,6]
```

## 解题思路、逆向遍历

每次把nums1和nums2的最后一个`有效元素`拿出来比较，谁大谁就往后放。

```cpp
void merge(vector<int>& nums1, int m, vector<int>& nums2, int n) {
  for (m -= 1, n -= 1; m >= 0 && n >= 0; ) {
    nums1[m+n+2] = nums1[m] > nums2[n] ? nums1[m--] : nums2[n--];
  }
  while (n >= 0) {
    nums1[m+n+2] = nums2[n--];
  }
}
```
- 时间复杂度：O(n)，最坏情况下是m+n次
- 空间复杂度：O(1)