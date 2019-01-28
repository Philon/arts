#include "solution.h"
#include <assert.h>
#include <string.h>

int main(int argc, char* argv[])
{
  assert(strStr("mississippi", "pi") == 9);
  assert(strStr("mississippi", "issi") == 1);
  assert(strStr("aabbccdd", "ee") == -1);
  assert(strStr("a", "") == 0);
  assert(strStr("", "a") == -1);
  return 0;
}