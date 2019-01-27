#include "solution.h"
#include <assert.h>

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

void assertListEqual(struct ListNode* a, struct ListNode* b) {
  while (a && b) {
    assert(a->val == b->val);
    a = a->next;
    b = b->next;
  }

  assert(a == NULL && b == NULL);
}

int main(int argc, char* argv[]) {
  assertListEqual(reverseKGroup(list("123"), 4), list("123"));
  assertListEqual(reverseKGroup(list("12345"), 2), list("21435"));
  assertListEqual(reverseKGroup(list("12345"), 3), list("32145"));
  assertListEqual(reverseKGroup(list("12345"), 4), list("43215"));
  assertListEqual(reverseKGroup(list(""), 0), list(""));
}