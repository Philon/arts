#include "solution.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

struct ListNode* list(int n, ...) {
  struct ListNode* list = (struct ListNode*)calloc(1, sizeof(struct ListNode));
  struct ListNode* node = list;

  va_list ap;
  va_start(ap, n);
  for (int i = 0; i < n; i++) {
    node->val = va_arg(ap, int);
    if (i + 1 == n) continue;
    node->next = (struct ListNode*)calloc(1, sizeof(struct ListNode));
    node = node->next;
  }
  va_end(ap);

  return n ? list : NULL;
}

void assertListEqual(struct ListNode* a, struct ListNode* b) {
  while (a && b) {
    assert(a->val == b->val);
    a = a->next;
    b = b->next;
  }

  assert(a == NULL && b == NULL);
}

int main(int argc, char* argv[])
{
  assertListEqual(mergeTwoLists(list(1, 1), list(2, 0, 1)), list(3, 0, 1, 1));
  assertListEqual(mergeTwoLists(list(2, 1, 5), NULL), list(2, 1, 5));
  return 0;
}