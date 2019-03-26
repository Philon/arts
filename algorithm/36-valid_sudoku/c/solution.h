#include <stdbool.h>

bool isValidSudoku(char** board, int boardRowSize, int boardColSize) {
  bool rows[9][9] = {false};
  bool cols[9][9] = {false};
  bool boxes[9][9] = {false};

  for (int i = 0; i < boardRowSize; i++) {
    for (int j = 0; j < boardColSize; j++) {
      if (board[i][j] == '.') {
        continue;
      }

      int num = board[i][j] - '0' - 1;
      int k = i / 3 * 3 + j / 3;
      if (rows[i][num] || cols[j][num] || boxes[k][num]) {
        return false;
      } else {
        rows[i][num] = cols[j][num] = boxes[k][num] = true;
      }
    }
  }

  return true;
}