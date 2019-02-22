#include "solution.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>

int str2array(char* s, int** arr) {
  int count = strlen(s);
  int* n = (int*)calloc(count, sizeof(int));
  for (int i = 0; i < count; i++) {
    n[i] = s[i] - '0';
  }

  *arr = n;
  return count;
}

void assertSolution(char* in, char* expect) {
  int* nums = NULL;
  int count = str2array(in, &nums);
  int* next = NULL;
  assert(count == str2array(expect, &next));
  nextPermutation(nums, count);
  for (int i = 0; i < count; i++) {
    assert(nums[i] == next[i]);
  }
}

int main(int argc, char* argv[])
{
  assertSolution("141", "411");
  assertSolution("547532", "552347");
  assertSolution("", "");
  assertSolution("1", "1");
  assertSolution("12", "21");
  assertSolution("6543", "3456");
  return 0;
}