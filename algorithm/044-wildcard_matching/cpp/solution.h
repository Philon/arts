#include <string>
#include <vector>

using namespace std;

class Solution {
public:
  bool isMatch(string s, string p) {
    int slen = s.length();
    int plen = p.length();
    vector<vector<bool>> dp(slen+1, vector<bool>(plen+1, false));

    dp[0][0] = true;
    for (int i = 1; i <= plen; i++) {
      dp[0][i] = p[i-1] == '*' && dp[0][i - 1];
    }

    for (int i = 1; i <= slen; i++) {
      for (int j = 1; j <= plen; j++) {
        char c = p[j-1];
        if (c == '*') {
          dp[i][j] = dp[i-1][j] || dp[i][j-1];
        } else if (c == '?') {
          dp[i][j] = dp[i-1][j-1];
        } else {
          dp[i][j] = s[i-1] == p[j-1] && dp[i-1][j-1];
        }
      }
    }

    return dp[slen][plen];
  }
};