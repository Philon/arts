#include <string>
#include <vector>

using namespace std;

class Solution {
public:
  string multiply(string num1, string num2) {
    int len1 = num1.size();
    int len2 = num2.size();
    vector<string> products;
    string result = "";

    reverse(num1);
    reverse(num2);

    for (int i = 0; i < len1; i++) {
      string product = "";
      int n1 = num1[i] - '0';
      int carry = 0;

      if (n1 < 2) {
        products.push_back(n1 == 1 ? num2 : "0");
        continue;
      }

      for (int j = 0; j < len2; j++) {
        int n2 = num2[j] - '0';
        int p = (n1 * n2) + carry;
        product += to_string(p % 10);
        carry = p / 10;
      }

      if (carry > 0) {
        product += to_string(carry);
      }

      products.push_back(product);
    }

    result = products[0];
    for (int i = 1; i < products.size(); i++) {
      if (products[i] == "0") {
        continue;
      }

      for (int j = 0; j < i; j++) {
        products[i] = '0' + products[i];
      }
      result = add(result, products[i]);
    }
    
    reverse(result);
    return result;
  }

private:
  void reverse(string& s) {
    int left = 0;
    int right = s.size() - 1;
    while (left < right) {
      char c = s[left];
      s[left] = s[right];
      s[right] = c;
      left++;
      right--;
    }
  }

  string add(string& a, string& b) {
    string sum = "";
    int carry = 0;
    for (int i = 0; a[i] || b[i] || carry; i++) {

      int n = carry;
      if (a[i]) n += (a[i] - '0');
      if (b[i]) n += (b[i] - '0');

      sum += to_string(n % 10);
      carry = n / 10;
    }

    return sum;
  }
};