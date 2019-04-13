#include "solution.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
  // assert(strcmp(countAndSay(4), "1211") == 0);
  for (int i = 1; i <= 30; i++) {
    char cmd[10240] = {0};
    sprintf(cmd, "echo '\"%s\",' >> data", countAndSay(i));
    system(cmd);
  }
  return 0;
}