#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char** generateParenthesis(int n, int* returnSize) {
  char** parentheses = (char**)calloc(n*n, sizeof(char*));
  int count = 0;
  int length = 2 * n + 1;

  if (n == 0) {
    parentheses[0] = (char*)calloc(1, sizeof(char));
    parentheses[0][0] = '\0';
    count = 1;
    return parentheses;
  }

  for (int i = 0; i < n; i++) {
    int leftCount = 0;
    char** lefts = generateParenthesis(i, &leftCount);
    for (int j = 0; j < leftCount; j++) {
      int rightCount = 0;
      char** rights = generateParenthesis(n - 1 - i, &rightCount);
      for (int k = 0; k < rightCount; k++) {
        char* p = (char*)calloc(length, sizeof(char));
        parentheses[count++] = p;
        char* l = lefts[j];
        char* r = rights[k];
        *p++ = '(';
        while (*l) *p++ = *l++;
        while (*r) *p++ = *r++;
        *p++ = ')';
        *p = '\0';

        free(rights[k]);
      }
      free(rights);
      free(lefts[j]);
    }
    free(lefts);
  }

  *returnSize = count;
  return parentheses;
}