#include <cmath>

class Solution {
public:
  int mySqrt(int x) {
    double root = (double)x / 2;
    unsigned long n = 0;
    unsigned long i = 2;
    do {
      n = root * root;
      i *= 2;
      if (n > x) {
        root -= (double)x / i;
      } else if (n < x) {
        root += (double)x / i;
      }
    } while (n != x);

    return (int)root;
  }
};