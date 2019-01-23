#include <stdlib.h>
#include <string.h>

void generate(char*** list, int* size, char* current, int open, int close, int n) {
  int i = open + close;
  if (i == (2 * n)) {
    // 如果括号放置完毕，在列表中追加存储区域，保存新字符串
    int l = *size;
    char** p = (char**)realloc(*list, sizeof(char*) * (l + 1));
    p[l] = (char*)calloc(2 * n + 1, sizeof(char));
    strcpy(p[l], current);

    *list = p;
    *size = l + 1;
    return;
  }

  if (open < n) {
    current[i] = '(';
    generate(list, size, current, open + 1, close, n);
  }

  if (close < open) {
    current[i] = ')';
    generate(list, size, current, open, close + 1, n);
  }
}

char** generateParenthesis(int n, int* returnSize) {
  char** parentheses = NULL;
  char* queue = (char*)calloc(2 * n + 1, sizeof(char));
  generate(&parentheses, returnSize, queue, 0, 0, n);

  return parentheses;
}
