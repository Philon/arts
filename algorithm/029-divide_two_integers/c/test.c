#include "solution.h"
#include <assert.h>

int main(int argc, char* argv[])
{
  assert(divide(10, 3) == 3);
  assert(divide(-2147483648, -1) == INTMAX);
  assert(divide(INTMIN, 1) == INTMIN);
  return 0;
}