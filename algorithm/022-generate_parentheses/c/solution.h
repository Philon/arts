#include <stdlib.h>
#include <string.h>

/**
 * Return an array of size *returnSize.
 * Note: The returned array must be malloced, assume caller calls free().
 */
char** generateParenthesis(int n, int* returnSize) {
  int length = n * 2 + 1;
  int count = (n - 1) * (n - 1) + 1;
  char** parentheses = (char**)calloc(count, sizeof(char*));
  char** p = parentheses;

  *p = (char*)calloc(length, sizeof(char));
  for (int i = 0; i < length - 1; i++) {
    (*p)[i] = i < n ? '(' : ')';
  }

  for (int i = 1; i < n; i++) {
    for (int j = n; j < length - 2; j++) {
      *++p = (char*)calloc(length, sizeof(char));
      strcpy(*p, parentheses[0]);
      (*p)[i] = ')';
      (*p)[j] = '(';
    }
  }

  *returnSize = count;
  return parentheses;
}