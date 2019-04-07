#include <stdbool.h>

bool check(char** board, int i, int j, char c) {
  for (int k = 0; k < 9; k++) {
    if (board[i][k] == c) return false;
    if (board[k][j] == c) return false;
    if (board[i-i%3+k/3][j-j%3+k%3] == c) return false;
  }

  return true;
}

bool solve(char** board, int row, int col) {
  if (col == 9) {
    row++;
    return row == 9 ? true : solve(board, row, 0);
  }

  if (board[row][col] != '.') {
    return solve(board, row, col+1);
  }

  for (char c = '1'; c <= '9'; c++) {
    if (check(board, row, col, c)) {
      board[row][col] = c;
      if (solve(board, row, col+1)) return true;
      board[row][col] = '.';
    }
  }

  return false;
}

void solveSudoku(char** board, int boardRowSize, int boardColSize) {
  solve(board, 0, 0);
}