#include <assert.h>
#include "solution.h"
#include <stdlib.h>
#include <stdarg.h>

void fillrow(char* row, char* s) {
  while (*s) {
    *row = *s != '.' ? *s - '0' : *s;
    row++, s++;
  }
}

int main(int argc, char* argv[]) {
  char** board = (char**)calloc(9, sizeof(char*));
  for (int i = 0; i < 9; i++) {
    board[i] = (char*)calloc(9, sizeof(char));
  }
  fillrow(board[0], "....65...");
  fillrow(board[1], ".2....4..");
  fillrow(board[2], "..3.7....");
  fillrow(board[3], "........5");
  fillrow(board[4], "...8..1..");
  fillrow(board[5], "..5....96");
  fillrow(board[6], "48.2.....");
  fillrow(board[7], ".....9..7");
  fillrow(board[8], "1........");

  assert(isValidSudoku(board, 9, 9) == true);
  board[6][8] = 3;
  assert(isValidSudoku(board, 9, 9) == false);
  return 0;
}