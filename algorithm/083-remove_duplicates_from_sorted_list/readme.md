# [从已排序数列表删除副本](https://leetcode.com/problems/remove-duplicates-from-sorted-list/)

给一个排序过的链表，删除所有重复的元素，仅保留一个。

```
Example 1:
Input: 1->1->2
Output: 1->2

Example 2:
Input: 1->1->2->3->3
Output: 1->2->3
```

## 解题思路

本题谈不上什么思路，很简单，一个个遍历，如果发现当前节点值与下个节点值相同，那就是副本，把下个节点移除就行了，否则直接跳过该节点。

```cpp
class Solution {
 public:
  ListNode* deleteDuplicates(ListNode* head) {
    ListNode* node = head;
    while (node) {
      if (node->next && node->next->val == node->val) {
        node->next = node->next->next;
      } else {
        node = node->next;
      }
    }

    return head;
  }
};
```
- 空间复杂度：O(n)，n为链表长度
- 时间复杂度：O(1)

PS: 好神奇，我的提交beats仅为72%，看了100%的代码实现，居然使用回溯法遍历，难道递归的函数调用比循环还快？🤔