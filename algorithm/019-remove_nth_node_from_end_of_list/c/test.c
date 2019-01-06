#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include "solution.h"

struct ListNode* list(int len, ...) {
  struct ListNode* list = (struct ListNode*)calloc(1, sizeof(struct ListNode));
  struct ListNode* node = list;

  va_list ap;
  va_start(ap, len);
  for (int i = 0; i < len; i++) {
    node->val = va_arg(ap, int);
    if (i + 1 == len) continue;
    node->next = (struct ListNode*)calloc(1, sizeof(struct ListNode));
    node = node->next;
  }
  va_end(ap);

  return len ? list : NULL;
}

void assertListEqual(struct ListNode* a, struct ListNode* b) {
  while (a && b) {
    assert(a->val == b->val);
    a = a->next;
    b = b->next;
  }

  assert(a == NULL && b == NULL);
}

int main(int argc, char* argv[]) {
  // 常规测试
  assertListEqual(removeNthFromEnd(list(5, 1, 2, 3, 4, 5), 2),
                  list(4, 1, 2, 3, 5));
  
  // 删除末尾测试
  assertListEqual(removeNthFromEnd(list(3, 1, 2, 3), 1), list(2, 1, 2));

  // 删除头测试
  assertListEqual(removeNthFromEnd(list(1, 1), 1), list(0));
  assertListEqual(removeNthFromEnd(list(2, 1, 2), 2), list(1, 2));
  return 0;
}