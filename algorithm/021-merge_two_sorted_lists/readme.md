# [合并两个已排序的链表](https://leetcode.com/problems/merge-two-sorted-lists/)

合并两个已经排序过的链表，并返回合并后的新链表。新的链表应该是之前两个链表节点的结合(不要分配内存)。

```
Example:
Input: 1->2->4, 1->3->4
Output: 1->1->2->3->4->4
```

# 我的思路

这道题在我脑海里就是“跑火车”，两列火车并排，但只有一条铁轨，一次只能跑一列火车。根据题意就知道了，每列火车车厢都有个数字编号，车头最小，顺着尾巴方向数字增大。

那么首先，把两列火车中，车头数字较大的火车拿到铁轨上“跑”：
1. 一边跑一边和另一列或者的每一列车厢比较
2. 一旦发现车头的数字小于旁边的车厢号了，火车头接到旁边列车车厢前边
2. 把旁边数字较大的车厢及其剩余部分拿到铁轨上跑，重复第一步

完事！

```c
struct ListNode* mergeTwoLists(struct ListNode* l1, struct ListNode* l2) {
  if (l1 == 0 || l2 == 0) {
    return l1 ? l1 : l2;
  }

  struct ListNode* head = l1->val < l2->val ? l1 : l2;
  struct ListNode* tmp = l1->val < l2->val ? l2 : l1;
  struct ListNode* node = head;
  while (node->next) {
    if (node->next->val > tmp->val) {
      struct ListNode* swp = node->next;
      node->next = tmp;
      tmp = swp;
    } else {
      node = node->next;
    }
  }

  node->next = tmp;
  return head;
}
```
- 时间复杂度：O(n)， 最坏情况下，n为两个链表的总长
- 空间复杂度：O(1)