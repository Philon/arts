#include <stdlib.h>

char* say(char* s) {
  if (s == NULL) {
    char* sequence = (char*)calloc(3, sizeof(char));
    sequence[0] = '1';
    sequence[1] = '\0';
    return sequence;
  }

  char* sequence = NULL;
  int count = 1;
  int len = 0;
  for (int i = 0; s[i] != 0; i++, count++) {
    if (s[i] != s[i+1]) {
      sequence = (char*)realloc(sequence, 3+len);
      sequence[len++] = count + '0';
      sequence[len++] = s[i];
      count = 0;
    }
  }

  free(s);
  sequence[len] = '\0';
  return sequence;
}

char* countAndSay(int n) {
  char* sequence = say(NULL);
  while (--n) {
    sequence = say(sequence);
  }

  return sequence;
}