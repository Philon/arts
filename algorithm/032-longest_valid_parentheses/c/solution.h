int longestValidParentheses(char* s) {
  int left = 0, right = 0;
  int longest = 0;
  char* p = s;
  for (; *p != '\0'; p++) {
    if (*p == '(') {
      left++;
    } else {
      right++;
    }

    if (left == right) {
      longest = right * 2 > longest ? right * 2 : longest;
    } else if (right > left) {
      left = right = 0;
    }
  }

  left = right = 0;
  for (p--; p >= s; p--) {
    if (*p == '(') {
      left++;
    } else {
      right++;
    }

    if (left == right) {
      longest = left * 2 > longest ? left * 2 : longest;
    } else if (left > right) {
      left = right = 0;
    }
  }

  return longest;
}