# [组合之和](https://leetcode.com/problems/combination-sum/)

给一组候选数字(`candidates`)且没有重复，以及一个目标数组(`target`)，找出所有候选数之和等于目标值的唯一组合。  
同一个候选数字可以被重复选择无数次。

注意：

所有数字(包括目标值)都为正整数。  
答案中不能包含重复的组合。

```
Example 1:
Input: candidates = [2,3,6,7], target = 7,
A solution set is:
[
  [7],
  [2,2,3]
]

Example 2:
Input: candidates = [2,3,5], target = 8,
A solution set is:
[
  [2,2,2,2],
  [2,3,3],
  [3,5]
]
```

## 网友思路：回溯法

这道题我没想太多，因为看完题目后脑子里大概有个思路：遍历`candidates`，从第一个开始重复相加，如果之和超过`target`就往回跳两个，然后从下个候选继续加，直到遍历完所有候选数字，这种方法自然是用递归来实现。

我从论坛里看到网友推荐的基本也是回溯法，所以就直接看答案了。
