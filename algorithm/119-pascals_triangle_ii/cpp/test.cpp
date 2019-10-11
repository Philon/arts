#include "solution.h"
#include <assert.h>

int main(int argc, char* argv[])
{
  Solution s;
  vector<int> row = {1, 7, 21, 35, 35, 21, 7, 1};
  assert(s.getRow(7) == row);
  return 0;
}