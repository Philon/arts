#ifndef bool
#define bool char
#endif

#include <string.h>
#include <stdlib.h>

bool isValid(char* s) {
  int len = strlen(s);
  char* brackets = (char*)calloc(len + 1, sizeof(char));
  char* p = brackets;

  while (*s) {
    if (*s == '(' || *s == '[' || *s == '{') {
      *p++ = *s;
    } else {
      p--;
      if (p < brackets) return 0;
      if (*p == '(' && *s != ')') return 0;
      if (*p == '[' && *s != ']') return 0;
      if (*p == '{' && *s != '}') return 0;
    }
    s++;
  }

  free(brackets);
  return p == brackets;
}