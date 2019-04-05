#include "solution.h"
#include <assert.h>
#include <stdlib.h>

void fillrow(char* row, char* s) {
  while (*s) {
    *row++ = *s++;
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

  const char* expect[] = {
    "534678912",
    "672195348",
    "198342567",
    "859761423",
    "426853791",
    "713924856",
    "961537284",
    "287419635",
    "345286179",
  };

  solveSudoku((char**)board, 9, 9);
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      assert(board[i][j] == expect[i][j]);
    }
  }

  return 0;
}