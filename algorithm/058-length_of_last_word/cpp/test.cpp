#include "solution.h"
#include <assert.h>

int main(int argc, char* argv[])
{
  Solution s;
  assert(s.lengthOfLastWord("Hello World") == 5);
  return 0;
}