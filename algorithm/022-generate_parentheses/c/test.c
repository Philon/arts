#include "solution.h"
#include <assert.h>
#include <stdarg.h>

void assertParentheses(int n, ...) {
  int count = 0;
  char** parentheses = generateParenthesis(n, &count);

  va_list ap;
  va_start(ap, n);
  // 充分遍历预计值和返回值，确保每个答案都得到匹配
  for (int i = 0; i < count; i++) {
    char* expect = va_arg(ap, char*);
    char** p = &parentheses[count];
    while (p --> parentheses) { // 😈
      if (*p && strcmp(*p, expect) == 0) {
        free(*p);
        *p = 0;
        break;
      }
    }
  }
  va_end(ap);

  // 所有返回值都应该被得到匹配
  for (int i = 0; i < count; i++) {
    assert(parentheses[i] == 0);
  }

  free(parentheses);
}

int main(int argc, char* argv[])
{
  assertParentheses(3, "((()))", "()()()", "(())()", "()(())", "(()())");
  return 0;
}