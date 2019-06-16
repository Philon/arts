#include "solution.h"
#include <assert.h>

int main(int argc, char* argv[])
{
  Solution s;
  assert(!s.isMatch("b", "?*?"));
  assert(s.isMatch("a", "a*"));
  assert(s.isMatch("abefcdgiescdfimde", "ab*cd?i*de"));
  assert(s.isMatch("lksdhf23ujjmsdy", "l*j?*sd?"));
  return 0;
}