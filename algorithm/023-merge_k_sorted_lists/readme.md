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

看到这个题第一反应就是想到[第21题](https://github.com/Philon/arts/tree/master/algorithm/021-merge_two_sorted_lists)，所以直接⌘C ⌘V，把代码改成下面这样，本地单元测试居然一次过了！满心欢喜提交后告诉老子内存没有对齐...妈个蛋💢！
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
- 空间复杂度：O(1)

*后来我上社区查了下，所谓的`runtime error: member access within misaligned ...`错误其实是个内存访问非法的问题。我十分不要脸的揣测一下：leetcode的测试用例的参数`lists`是一个指针数组，当且仅当`listsSize == 0`的时候，即参数lists是一个长度为0的数组，那么`lists[0]`应该是一个空指针？*
- *我本地的单元测试的内存分配用`calloc()`——`lists[0] == NULL`成立*
- *leetcode服务器可能采用`malloc()`——`lists[0]`是个随机数*

*结果就是导致在官方的机器上，我返回了一个非空(但是无效)指针。唉，这锅，我不想背👋！*

解决所谓的内存对齐错误很简单，在函数最开始加一条判断即可：
```c
// 当链表数组长度为零时，确保返回一个空指针
if (listsSize == 0) {
  return NULL;
}
```

当然，提交之前我心里很清楚，这种效率太低了，但看到beats60%的时候，当时我都震惊了——这届码农不行！

后面想要降低时间复杂度，又尝试了几种办法都无功而返，无奈打开官方解答，看到最后一种方案就是“相邻的两对链表合并”，我擦！这是我距离正确答案最近的一次😅，就这么擦身而过，然后果断把上面的“一一合并”改成了“两两合并”：

```c
struct ListNode* mergeKLists(struct ListNode** lists, int listsSize) {
  int interval = 1;
  while (interval < listsSize) {
    for (int i = 0; i < listsSize; i += interval * 2) {
      struct ListNode* next = (i+interval) < listsSize ? lists[i+interval] : NULL;
      lists[i] = mergeTwoLists(lists[i], next);
    }
    interval *= 2;
  }
  
  return listsSize ? lists[0] : NULL;
}
```
- 时间复杂度：O(n * listsSize)，n是链表数组当中最长的那根
- 空间复杂度：O(1)

---
以下翻译自[官方solution](https://leetcode.com/problems/merge-k-sorted-lists/solution/)

## 方案一、粗暴法

### 原理及算法

- 遍历所有的链表，然后把每个链表的节点存到一个数组中
- 根据数组中每个元素的node->val进行排序
- 创建一个新链表，把刚才排序好的数组放入其中

关于排序，你可以通过[这里](https://www.cs.cmu.edu/~adamchik/15-121/lectures/Sorting%20Algorithms/sorting.html)获取更多相关算法的信息。

以下是Python的实现：
```python
class Solution(object):
    def mergeKLists(self, lists):
        """
        :type lists: List[ListNode]
        :rtype: ListNode
        """
        self.nodes = []
        head = point = ListNode(0)
        for l in lists:
            while l:
                self.nodes.append(l.val)
                l = l.next
        for x in sorted(self.nodes):
            point.next = ListNode(x)
            point = point.next
        return head.next
```
- 时间复杂度：O(N*logN)，N表示(所有链表)节点的数量
- 空间复杂度：O(N)

## 方案二、一对一比较

### 算法

- 比较数组中的每个节点(头部节点)，然后取出最小那一个，(头部移至下一个节点)
- 把选中的节点扩展到一个链表尾部

### 复杂度分析

- 时间复杂度：O(kN)，k是链表数量，N是所有节点数量
- 空间复杂度：O(N)，创建新链表的情况下，如果仅仅合并到其中一个链表，则是O(1)

## 方案三、通过优先级队列优化方案二

### 算法

几乎和上面的方案一样，只是通过优先级队列的方式优化了一下。具体可以通过[这里](https://en.wikipedia.org/wiki/Priority_queue)了解优先级队列算法信息。

以下是Python的实现：
```python
from Queue import PriorityQueue

class Solution(object):
    def mergeKLists(self, lists):
        """
        :type lists: List[ListNode]
        :rtype: ListNode
        """
        head = point = ListNode(0)
        q = PriorityQueue()
        for l in lists:
            if l:
                q.put((l.val, l))
        while not q.empty():
            val, node = q.get()
            point.next = ListNode(val)
            point = point.next
            node = node.next
            if node:
                q.put((node.val, node))
        return head.next
```

## 方案四、一一合并

把合并k个链表转换成合并(k-1)次2个链表的问题，然后看看[merge 2 list](https://leetcode.com/problems/merge-two-sorted-lists/description/)这道题就明白了。

## 方案五、两两合并

### 原理及算法

这个方案相比于上边那种要改善太多了。我们没必要把所有节点都重复遍历那么多遍：
- 将k个链表分成对，然后合并每一对
- 当成对合并后，k个链表合并为k/2个，并且长度为2N/k，以此类推，k/4，k/8...
- 重复以上操作直至我们最终剩下一个链表

这样，我们总在遍历N个节点的链表对并将其合并，需要重复操作log2k次。

![](https://leetcode.com/problems/merge-k-sorted-lists/Figures/23/23_divide_and_conquer_new.png)

以下是python实现：
```python
class Solution(object):
    def mergeKLists(self, lists):
        """
        :type lists: List[ListNode]
        :rtype: ListNode
        """
        amount = len(lists)
        interval = 1
        while interval < amount:
            for i in range(0, amount - interval, interval * 2):
                lists[i] = self.merge2Lists(lists[i], lists[i + interval])
            interval *= 2
        return lists[0] if amount > 0 else lists

    def merge2Lists(self, l1, l2):
        head = point = ListNode(0)
        while l1 and l2:
            if l1.val <= l2.val:
                point.next = l1
                l1 = l1.next
            else:
                point.next = l2
                l2 = l1
                l1 = point.next.next
            point = point.next
        if not l1:
            point.next=l2
        else:
            point.next=l1
        return head.next
```
- 时间复杂度：O(N * log2k)，k是链表数量，N是每次合并后的节点数
- 空间复杂度：O(1)