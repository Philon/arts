#include "solution.h"
#include <assert.h>

int main(int argc, char* argv[])
{
  Solution s;
  vector<int> height = {0,1,0,2,1,0,1,3,2,1,2,1};
  assert(s.trap(height) == 6);
  return 0;
}