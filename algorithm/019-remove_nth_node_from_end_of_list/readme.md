# [从列表移除倒数第N个节点](https://leetcode.com/problems/remove-nth-node-from-end-of-list/)

给一个链表，从链表删除末尾第n个节点，并返回表头。

```
Example:
给一个链表: 1->2->3->4->5, and n = 2.
删除倒数第2个节点后，链表就变成了 1->2->3->5。
```
**注意**:  
给定的`n`总是有效的。

**挑战**:  
有种一波流啊？！

## 我的思路：

呐，最开始呢大家都会想的～先遍历一遍链表，取得长度，然后再遍历一遍跑到倒数第n的节点前面，移除，搞定！

可偏偏题目问敢不敢一步到位。当时我就不乐意了，你都不知道总长度，哪儿来的“倒数第n个”。想啊想，就这么一哆嗦，想出来啦：

1. 我虽然一开始不知道链表长度是多少，但它至少有n个呀
2. 我只需要遍历一遍链表，把末尾的n个节点全部缓存
3. 缓存的“起始”节点不就是倒数第n个吗，干掉它！
4. 为了干掉第n个节点，我必须多缓存它之前的一个节点，+1
5. 为了避免节点处于缓存首/尾的情况，左右两边各增加一个尾/首副本，`尾-首-中-尾-首`的形式，+2

```c
struct ListNode* removeNthFromEnd(struct ListNode* head, int n) {
  int len = n + 3;
  struct ListNode** cache = (struct ListNode**)calloc(len, sizeof(void*));
  struct ListNode* node = head;
  int i = 1;

  while (node) {
    cache[i] = node;
    node = node->next;
    i = (i <= n) ? i+1 : 1;
  }

  cache[0] = cache[n];
  cache[len-1] = cache[1];

  if (cache[i] == NULL) { // 移除表头
    head = head->next;
  } else if (n == 1) { // 移除表尾
    cache[i]->next = NULL;
  } else { // 移除中间
    cache[i]->next = cache[i+1]->next;
  }

  free(cache);
  return head;
}
```
- 时间复杂度：O(n)，遍历了一遍链表
- 空间复杂度：O(n)，入参`n+3`

## 官方方案一：两步到位

### 概要
我们要注意，这道题可以简单地理解为减少某个节点：从链表开始移除第(L-n+1)那个。所以只要找到链表长度L，这道题就so easy了。

### 算法

首先我们要增加一个辅助的“dummy”节点，放到链表头部。这个“dummy”节点只是为了简化链表中只有一个节点，或者删除表头本身等操作。第一次遍历，我们要找出链表长度。然后重新定位到dummy节点，然后移除从它开始的第(L-n)的下个节点。然后再把链表给重新“接”回去。即(L-n)的next指向(L-n+2)的节点。

![Figure 1. Remove the L - n + 1 th element from a list.](https://leetcode.com/media/original_images/19_Remove_nth_node_from_end_of_listA.png)

```java
public ListNode removeNthFromEnd(ListNode head, int n) {
    ListNode dummy = new ListNode(0);
    dummy.next = head;
    int length  = 0;
    ListNode first = head;
    while (first != null) {
        length++;
        first = first.next;
    }
    length -= n;
    first = dummy;
    while (length > 0) {
        length--;
        first = first.next;
    }
    first.next = first.next.next;
    return dummy.next;
}
```
- 时间复杂度：O(L)，两次遍历链表，第一次移除长度，第二次找第(L-n)个节点。一共花费2L-n次操作。
- 空间复杂度：O(1)

## 官方方案二：一步到位

### 算法

上边的算法可以被充分改造为一步到位。作为代替，我们要增加两个节点。第一个从链表头开始一个一个往前移，第二个按兵不动。直到，两个节点相隔距离为n。现在开始我们保持两个节点之间的这段“空白”同时移动，直到第一个节点到达链表尾巴。好了，第二个节点已经在倒数第n个节点处了。干掉它！

![Figure 2. Remove the nth element from end of a list.](https://leetcode.com/media/original_images/19_Remove_nth_node_from_end_of_listB.png)

```java
public ListNode removeNthFromEnd(ListNode head, int n) {
    ListNode dummy = new ListNode(0);
    dummy.next = head;
    ListNode first = dummy;
    ListNode second = dummy;
    // Advances first pointer so that the gap between first and second is n nodes apart
    for (int i = 1; i <= n + 1; i++) {
        first = first.next;
    }
    // Move first to the end, maintaining the gap
    while (first != null) {
        first = first.next;
        second = second.next;
    }
    second.next = second.next.next;
    return dummy.next;
}
```
- 时间复杂度：O(L)，只遍历了一遍链表，长度为L
- 空间复杂度：O(1)，仅仅用了一个常数，扩展空间