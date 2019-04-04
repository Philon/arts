#include "solution.h"
#include <assert.h>
#include <stdlib.h>

const char expect[9][9] = {
  {5, 3, 4, 6, 7, 8, 9, 1, 2},
  {6, 7, 2, 1, 9, 5, 3, 8, 4},
  {1, 9, 8, 3, 4, 2, 5, 6, 7},
  {8, 5, 9, 7, 6, 1, 4, 2, 3},
  {4, 2, 6, 8, 5, 3, 7, 9, 1},
  {7, 1, 3, 9, 2, 4, 8, 5, 6},
  {9, 6, 1, 5, 3, 7, 2, 8, 4},
  {2, 8, 7, 4, 1, 9, 6, 3, 5},
  {3, 4, 5, 2, 8, 6, 1, 7, 9},
};

void fillrow(char* row, char* s) {
  while (*s) {
    *row = *s != '.' ? *s - '0' : *s;
    row++, s++;
  }
}

int main(int argc, char* argv[])
{
  char** board = (char**)calloc(9, sizeof(char*));
  for (int i = 0; i < 9; i++) {
    board[i] = (char*)calloc(9, sizeof(char));
  }
  fillrow(board[0], "53..7....");
  fillrow(board[1], "6..195...");
  fillrow(board[2], ".98....6.");
  fillrow(board[3], "8...6...3");
  fillrow(board[4], "4..8.3..1");
  fillrow(board[5], "7...2...6");
  fillrow(board[6], ".6....28.");
  fillrow(board[7], "...419..5");
  fillrow(board[8], "....8..79");

  solveSudoku((char**)board, 9, 9);
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      assert(board[i][j] == expect[i][j]);
    }
  }

  return 0;
}