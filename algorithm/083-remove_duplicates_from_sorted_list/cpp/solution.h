#include <iostream>
#include <vector>

struct ListNode {
  int val;
  ListNode *next;
  ListNode(int x) : val(x), next(NULL) {}
};

class Solution {
 public:
  ListNode* deleteDuplicates(ListNode* head) {
    ListNode* node = head;
    while (node) {
      if (node->next && node->next->val == node->val) {
        node->next = node->next->next;
      } else {
        node = node->next;
      }
    }

    return head;
  }
};