#include "solution.h"
#include <assert.h>
#include <stdarg.h>

struct ListNode* list(char* data) {
  struct ListNode results = {.val = 0, .next = 0};
  struct ListNode* node = &results;

  while (*data) {
    node->next = (struct ListNode*)calloc(1, sizeof(struct ListNode));
    node = node->next;
    node->val = *data++ - '0';
  }

  return results.next;
}

struct ListNode** lists(int count, ...) {
  struct ListNode** results = (struct ListNode**)calloc(count, sizeof(struct ListNode*));
  va_list ap;
  va_start(ap, count);
  for (int i = 0; i < count; i++) {
    results[i] = list(va_arg(ap, char*));
  }
  va_end(ap);

  return results;
}

void assertListEqual(struct ListNode* a, struct ListNode* b) {
  while (a && b) {
    assert(a->val == b->val);
    a = a->next;
    b = b->next;
  }

  assert(a == NULL && b == NULL);
}

#define merge(n, ...) mergeKLists(lists(n, __VA_ARGS__), n)
int main(int argc, char* argv[])
{
  assertListEqual(merge(3, "145", "134", "26"), list("11234456"));
  assertListEqual(merge(0, ""), list(""));

  return 0;
}