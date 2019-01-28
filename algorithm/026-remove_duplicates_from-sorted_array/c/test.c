#include "solution.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int* array(const char* s) {
  int* nums = calloc(strlen(s), sizeof(int));
  int* n = nums;
  while (*s) {
    *n++ = *s++ - '0';
  }

  return nums;
}

void assertSolution(const char* in, const char* expect) {
  int* nums = array(in);
  int* e = array(expect);
  int rlen = removeDuplicates(nums, strlen(in));

  assert(rlen == strlen(expect));
  for (int i = 0; i < rlen; i++) {
    assert(nums[i] == e[i]);
  }
}

int main(int argc, char* argv[])
{
  assertSolution("11223344", "1234");
  assertSolution("11", "1");
  assertSolution("1", "1");
  assertSolution("", "");
  return 0;
}