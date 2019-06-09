#include "solution.h"
#include <assert.h>

int main(int argc, char* argv[])
{
  Solution s;
  assert(s.multiply("123", "456") == "56088");
  assert(s.multiply("1234758367", "0") == "0");
  return 0;
}