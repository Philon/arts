# [组合之和II](https://leetcode.com/problems/combination-sum-ii/)

给一组候选数字(`candidates`)且没有重复，以及一个目标数组(`target`)，找出所有候选数之和等于目标值的唯一组合。  

候选数字中的每个数字**仅用于一次**到组合中。

注意：

所有数字(包括目标值)都为正整数。  
答案中不能包含重复的组合。

```
Example 1:
Input: candidates = [10,1,2,7,6,1,5], target = 8,
A solution set is:
[
  [1, 7],
  [1, 2, 5],
  [2, 6],
  [1, 1, 6]
]

Example 2:
Input: candidates = [2,5,2,1,2], target = 5,
A solution set is:
[
  [1,2,2],
  [5]
]
```

## 回溯法

本题和第039题区别不大，唯一限制就是候选数字在每个组合里只能用一次，所以只需要在039题的基础之上加一个if判断是否重复即可。