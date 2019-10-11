#include <vector>

using namespace std;

class Solution {
public:
  vector<vector<int>> generate(int numRows) {
    vector<vector<int>> rows;

    for (int i = 0; i < numRows; i++) {
      vector<int> r(i+1, 1);
      for (int j = 1; j < i; j++) {
        r[j] = rows[i-1][j-1] + rows[i-1][j];
      }
      rows.push_back(r);
    }

    return rows;
  }
};