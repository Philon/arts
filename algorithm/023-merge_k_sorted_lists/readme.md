# [合并K个已排序链表](https://leetcode.com/problems/merge-k-sorted-lists/)

合并K个已排序的链表集合，并返回为一个链表。分析并描述它的复杂度。

```
Example:

Input:
[
  1->4->5,
  1->3->4,
  2->6
]
Output: 1->1->2->3->4->4->5->6
```

## 我的思路

看到这个题第一反应就是想到[第21题](https://github.com/Philon/arts/tree/master/algorithm/021-merge_two_sorted_lists)，所以直接⌘C ⌘V，把代码改成下面这样，本地单元测试居然一次过了！满心欢喜提交后告诉老子内存没有对齐...妈个蛋！
```c
// 此处省略mergeTwoLists() ...
struct ListNode* mergeKLists(struct ListNode** lists, int listsSize) {
  struct ListNode* result = *lists;
  for (int i = 1; i < listsSize; i++) {
    result = mergeTwoLists(result, lists[i]);
  }

  return result;
}
```
- 时间复杂度：O(n * listsSize)，n是所有链表长度之和
- 空间复杂度：o(1)