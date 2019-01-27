#include <stdlib.h>

struct ListNode {
  int val;
  struct ListNode* next;
};

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

struct ListNode* swapPairs(struct ListNode* head) {
  if (head == NULL || head->next == NULL) {
    return head;
  }

  struct ListNode result = { .val=0, .next=head };
  head = swapNextPair(&result);
  while (head) {
    head = swapNextPair(head);
  }

  return result.next;
}