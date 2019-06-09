#include <string>
#include <vector>

using namespace std;

class Solution {
public:
  string multiply(string num1, string num2) {
    if (num1 == "0" || num2 == "0") {
      return "0";
    }
    
    int len1 = num1.size();
    int len2 = num2.size();
    vector<int> products(len1 + len2, 0);

    for (int i = len1-1; i >= 0; i--) {
      int n1 = num1[i] - '0';
      for (int j = len2-1; j >= 0; j--) {
        int p = n1 * (num2[j] - '0');
        p += products[i+j+1];
        products[i+j+1] = p % 10;
        products[i+j] += p / 10;
      }
    }

    string result = "";
    int len = len1 + len2;
    int i = 0;
    while (products[i] == 0) i++;
    for (; i < len; i++) {
      result += products[i] + '0';
    }

    return result;
  }
};