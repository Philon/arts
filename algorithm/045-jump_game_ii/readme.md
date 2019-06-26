# [Jump Game II](https://leetcode.com/problems/jump-game-ii/)

给定一个非负整数数组，你最初的位置是数组的第一个索引。  
数组中的每个元素都相当于你从该位置起，能跳跃的最大长度。  
你的目标是用最小跳跃次数到达最后一个索引。

```
Example:
Input: [2,3,1,1,4]
Output: 2
解释: 跳跃到最后一个索引的最小跳跃次数是2。第1步是从索引0跳到1，然后就获得了3次跳跃，就可以到达末尾索引了。
```

注意: 你可以假定总能到达最后的索引。


## 贪心算法

其实这种题一看就有个老思路——动态规划/回溯发，反正就是通过遍历每种可能的方案，然后决策出最优解。不过这种方案想也知道，它的时间复杂度往往是O(n²)。我是看了帖子才知道可以通过BFS(广度优先搜索)和贪心算法来解决的。不论是BFS还是贪心，主要用于最短路径规划，例如从地图的A点到B点，有多个岔路口，通过该算法找到一条行程最短的路线。

```cpp
int jump(vector<int>& nums) {
  if (nums.size() < 2) {
    return 0;
  }

  int count = 0;
  int last = 0; // 表示能跳到的最后位置
  int current = 0; // 表示当前位置

  for (int i = 0; i < nums.size() - 1; i++) {
    last = max(last, nums[i] + i);
    if (i == current) {
      count++;
      current = last;
    }
  }

  return count;
}
```
- 时间复杂度：O(n)
- 空间复杂度：O(1)