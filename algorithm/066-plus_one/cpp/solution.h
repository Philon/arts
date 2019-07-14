#include <vector>

using namespace std;

class Solution {
public:
  vector<int> plusOne(vector<int>& digits) {
    int carry = 1;

    for (int i = digits.size() - 1; i >= 0 && carry > 0; i--) {
      digits[i] += carry;
      carry = digits[i] / 10;
      digits[i] = digits[i] % 10;
    }

    if (carry > 0) {
      digits.insert(digits.begin(), carry);
    }

    return digits;
  }
};