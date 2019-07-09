#include "solution.h"
#include <assert.h>

int main(int argc, char* argv[])
{
  Solution s;
  assert(s.addBinary("1", "11") == "100");

  return 0;
}