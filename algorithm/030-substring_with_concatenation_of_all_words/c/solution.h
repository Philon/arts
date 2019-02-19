#include <stdlib.h>
#include <memory.h>

int match(char* s, char** words, int wordsSize, int* matched) {
  for (int i = 0; i < wordsSize; i++) {
    if (matched[i]) { continue; }
    char* p = s;
    char* q = words[i];
    while (*p && *q && *p == *q) { p++, q++; }
    if (*q == '\0') {
      matched[i] = p - s;
      return i;
    }
  }

  return -1;
}

int* findSubstring(char* s, char** words, int wordsSize, int* returnSize) {
  int* result = (int*)calloc(1, sizeof(int));
  int count = 0;
  int matchCount = 0;
  int* matchesIndex = (int*)calloc(wordsSize, sizeof(int));
  int* matchesLength = (int*)calloc(wordsSize, sizeof(int));

  for (int i = 0; s[i] != '\0'; i++) {
    int index = match(&s[i], words, wordsSize, matchesLength);
    if (index < 0) {
      memset(matchesIndex, 0, sizeof(int) * wordsSize);
      memset(matchesLength, 0, sizeof(int) * wordsSize);
      i -= matchCount ? 1 : 0;
      matchCount = 0;
      continue;
    }

    matchesIndex[matchCount++] = index;
    if (matchCount == 1) {
      result[count] = i;
    } else if (matchCount == wordsSize) {
      count++;
      result = (int*)realloc(result, sizeof(int) * count);
      result[count] = i + matchesLength[index];
      matchesLength[matchesIndex[0]] = 0;
      for (int j = 1; j < matchCount; j++) {
        result[count] -= matchesLength[matchesIndex[j]];
        matchesIndex[j-1] = matchesIndex[j];
      }
      matchCount--;
    }

    i += matchesLength[index] - 1;
  }

  free(matchesIndex);
  free(matchesLength);
  *returnSize = count;
  return result;
}