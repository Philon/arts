struct ListNode {
  int val;
  struct ListNode* next;
};

struct ListNode* removeNthFromEnd(struct ListNode* head, int n) {
  int len = n + 3;
  struct ListNode** cache = (struct ListNode**)calloc(len, sizeof(void*));
  struct ListNode* node = head;
  int i = 0;

  while (node) {
    cache[i] = node;
    node = node->next;
    i = (i <= n) ? i+1 : 1;
  }

  cache[0] = cache[n];
  cache[len-1] = cache[1];

  if (cache[i]) {
    // 正常移除
    cache[i]->next = n == 1 ? NULL : cache[i+1]->next;
  } else {
    // 移除表头
    fprintf(stderr, "remove head\n");
    head = NULL;
  }

  free(cache);
  return head;
}