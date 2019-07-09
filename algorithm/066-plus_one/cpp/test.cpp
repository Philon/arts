#include "solution.h"
#include <assert.h>

int main(int argc, char* argv[])
{
  Solution s;
  vector<int> input = {0};
  vector<int> output = {1};
  assert(s.plusOne(input) == output);
  input = {9, 9, 9};
  output = {1, 0, 0, 0};
  assert(s.plusOne(input) == output);

  return 0;
}