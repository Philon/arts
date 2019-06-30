# [全排列](https://leetcode.com/problems/permutations/)

给一套没有重复的整数集合，返回所有可能的全排列

```
Example:
Input: [1,2,3]
Output:
[
  [1,2,3],
  [1,3,2],
  [2,1,3],
  [2,3,1],
  [3,1,2],
  [3,2,1]
]
```

## 解题思路：回溯法

本题就是数学中的排列组合思想，程序实现的思路基本就是深度遍历进而尝试所有可能，比如nums为集合[1, 2, 3, 4]，其过程是按住元素`1`不动，排列之后的`2,3,4`，然后把`2`放到最前，排列之后的`1,3,4`，然后是`3`...

那么以第一步为例，之后的元素`2,3,4`怎么排列？按住元素`2`不动，排列之后的`3,4`...而所谓的排列其实就是把两个元素交换一下。

大体思路就是遍历+交换，而所谓遍历其实都是相同的处理方式，用递归(回溯法)再合适不过。

```cpp
class Solution {
public:
  vector<vector<int>> permute(vector<int>& nums) {
    vector<vector<int>> permutations;
    permute(nums, permutations, 0);
    return permutations;
  }

private:
  void permute(vector<int>& nums, vector<vector<int>>& permutations, int i) {
    if (i == nums.size()) {
      permutations.push_back(nums);
    }

    for (int j = i; j < nums.size(); j++) {
      swap(nums[i], nums[j]);
      permute(nums, permutations, i + 1);
      swap(nums[i], nums[j]);
    }
  }
};
```
- 时间复杂度：O(n!)，一个排列有多少种，当然是n的阶乘啦！
- 空间复杂度：O(n!)，每种排列都意味着要分配一个vector