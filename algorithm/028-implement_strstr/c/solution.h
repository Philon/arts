int strStr(char* haystack, char* needle) {
  char* p = haystack;
  while (*needle) {
    char* q = needle;
    while (*p && *p != *q) p++;
    
    int i = 0;
    while (p[i] && p[i] == q[i]) i++;

    if (q[i] == '\0') {
      return p - haystack;
    } else if (p[i] == '\0' && q[i]) {
      return -1;
    } else {
      p++;
    }
  }

  return 0;
}