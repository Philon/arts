#include <vector>

using namespace std;

class Solution {
public:
  vector<int> getRow(int rowIndex) {
    vector<int> row(rowIndex+1, 1);

    for (int i = 2; i <= rowIndex; i++) {
      vector<int> above = row;
      for (int j = 1; j < i; j++) {
        row[j] = above[j-1] + above[j];
      }
    }

    return row;
  }
};