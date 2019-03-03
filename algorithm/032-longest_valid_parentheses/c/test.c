#include "solution.h"
#include <assert.h>

int main(int argc, char* argv[])
{
  assert(longestValidParentheses("((()()(()))") == 10);
  return 0;
}