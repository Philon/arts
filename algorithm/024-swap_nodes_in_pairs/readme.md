# [交换节点对](https://leetcode.com/problems/swap-nodes-in-pairs/)

给一个链表，以相邻的每两个节点为一对进行交换，并返回表头。
```
Example:
输入 1->2->3->4, 返回 2->1->4->3.
```

注意：  
你的算法分配空间只能是个常数。  
不要去修改节点内的数值，仅仅是修改节点自身。

## 我的思路

这道题乍一看好像很简单，如果只是简单的把两个节点的指向互换一下，难度连easy都不够格，其实真正的坑就在于“交换一对节点”，当前这对调换完毕，可他们之前的节点指向依然没变啊！这才是真正不好处理的地方。

另外，按照题目最后的约束，“把节点存入数组再交换，或者仅交换内容”这种粗暴法是不可能的了。

所以最安全的做法是：交换**当前节点的下一对**，然后让当前节点指向下一对交换后的结果即可。先来实现交换下一对节点：
```c
// 交换head所指向的下一对节点，并返回交换后的末尾节点
struct ListNode* swapNextPair(struct ListNode* head) {
  struct ListNode* a = head->next;
  if (a && a->next) {
    struct ListNode* b = a->next;
    head->next = b;
    a->next = b->next;
    b->next = a;
  }

  return a;
}
```

有了这个函数之后剩下的就好办了，因为每次交换完成都能拿到“末尾节点”，而这个末尾节点正好是再下一对的头节点，所以主函数可以这么写：
```c
struct ListNode* swapPairs(struct ListNode* head) {
  if (head == NULL || head->next == NULL) {
    return head;
  }

  // result仅仅用来保存处理过后的链表头部
  struct ListNode result = { .val=0, .next=head };
  head = swapNextPair(&result);
  while (head) {
    head = swapNextPair(head);
  }

  return result.next;
}
```
- 时间复杂度：O(N)，N为链表节点数
- 空间复杂度：O(1)