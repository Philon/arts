#include "solution.h"
#include <assert.h>
#include <vector>

ListNode* list(std::vector<int> nums) {
  ListNode* head = new ListNode(nums[0]);
  ListNode** next = &head->next;
  for (int i = 1; i < nums.size(); i++) {
    *next = new ListNode(nums[i]);
    next = &(*next)->next;
  }
  *next = NULL;

  return head;
}

void assertEqual(ListNode* l1, ListNode* l2) {
  while (l1 && l2) {
    assert(l1->val == l2->val);
    l1 = l1->next;
    l2 = l2->next;
  }

  assert(l1 == NULL);
  assert(l2 == NULL);
}

int main(int argc, char* argv[])
{
  Solution s;
  auto input = list((std::vector<int>){1, 1, 2, 3, 3});
  auto expect = list((std::vector<int>){1, 2, 3});
  assertEqual(s.deleteDuplicates(input), expect);
  return 0;
}