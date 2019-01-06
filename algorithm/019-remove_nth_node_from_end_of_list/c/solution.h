struct ListNode {
  int val;
  struct ListNode* next;
};

struct ListNode* removeNthFromEnd(struct ListNode* head, int n) {
  int len = n + 3;
  struct ListNode** cache = (struct ListNode**)calloc(len, sizeof(void*));
  struct ListNode* node = head;
  int i = 1;

  while (node) {
    cache[i] = node;
    node = node->next;
    i = (i <= n) ? i+1 : 1;
  }

  cache[0] = cache[n];
  cache[len-1] = cache[1];

  if (cache[i] == NULL) { // 移除表头
    head = head->next;
  } else if (n == 1) { // 移除表尾
    cache[i]->next = NULL;
  } else { // 移除中间
    cache[i]->next = cache[i+1]->next;
  }

  free(cache);
  return head;
}