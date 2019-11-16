#include "solution.h"
#include <assert.h>

int main(int argc, char* argv[])
{
  Solution s;
  vector<vector<int>> levels = {
    {1},
    {1, 1},
    {1, 2, 1},
    {1, 3, 3, 1},
    {1, 4, 6, 4, 1},
    {1, 5, 10, 10, 5, 1},
    {1, 6, 15, 20, 15, 6, 1},
    {1, 7, 21, 35, 35, 21, 7, 1}
  };
  assert(s.generate(8) == levels);
  return 0;
}