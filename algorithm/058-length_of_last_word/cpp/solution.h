#include <string>

using namespace std;

class Solution {
public:
  int lengthOfLastWord(string s) {
    const char* p = s.c_str();
    const char* q = p + s.length() - 1;
    int lastLength = 0;

    while (*q == ' ') q--;
    while (q >= p && *q != ' ') {
      q--;
      lastLength++;
    }

    return lastLength;
  }
};