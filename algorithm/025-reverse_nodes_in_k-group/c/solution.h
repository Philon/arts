#include <stdlib.h>

struct ListNode {
  int val;
  struct ListNode* next;
};

struct ListNode* reverseKGroup(struct ListNode* head, int k) {
  struct ListNode* node = head;
  for (int i = 1; i < k && node; i++) {
    struct ListNode* next = node->next;
    if (next == NULL) {
      head = reverseKGroup(head, i);
      return head;
    }

    node->next = next->next;
    next->next = head;
    head = next;
  }

  if (node) {
    node->next = reverseKGroup(node->next, k);
  }

  return head;
}