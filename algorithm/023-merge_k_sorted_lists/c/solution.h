#include <stdlib.h>

struct ListNode {
  int val;
  struct ListNode* next;
};

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