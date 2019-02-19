#include "solution.h"
#include <assert.h>

int main(int argc, char* argv[])
{
  char* s = "wordgoodgoodgoodbestword";
  char* words[] = {"word","good","best","good"};
  int count = 0;
  int* result = findSubstring(s, words, 3, &count);
  int expect[] = {6, 9, 12};

  assert(count == (sizeof(expect) / sizeof(int)));
  for (int i = 0; i < count; i++) {
    assert(result[i] == expect[i]);
  }

  return 0;
}