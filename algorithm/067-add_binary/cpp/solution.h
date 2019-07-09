#include <string>

using namespace std;

class Solution {
public:
  string addBinary(string a, string b) {
    string sum = "";
    int carry = 0;

    for (int i = a.size()-1, j = b.size()-1; i >= 0 || j >= 0; i--, j--) {
      int n = carry;
      if (i >= 0) n += a[i] - '0';
      if (j >= 0) n += b[j] - '0';
      sum += (n % 2) + '0';
      carry = n / 2;
    }

    if (carry) {
      sum += '1';
    }

    reverse(sum.begin(), sum.end());
    return sum;
  }
};