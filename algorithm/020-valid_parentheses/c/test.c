#include "solution.h"
#include <assert.h>

int main(int argc, char* argv[])
{
  // 测试正常情况
  assert(isValid("{[()][()()]}{}"));
  // 测试空字符串
  assert(isValid(""));
  // 测试非法情况
  assert(isValid("[{}({})}") == 0);
  assert(isValid(")(") == 0);
  assert(isValid("[") == 0);
  return 0;
}