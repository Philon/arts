#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

int main(int argc, char* argv[])
{
  if (argc < 3) {
    fprintf(stderr, "\n./rgbled_test <r|g|b> <0|1>\n\n");
    exit(0);
  }

  int fd = open("/dev/rgbled", O_RDWR);
  if (fd < 0) {
    perror("open device");
    return -1;
  }

  switch (argv[1][0]) {
  case 'r':
  case 'R':
    ioctl(fd, 2, atoi(argv[2]));
    break;
  case 'g':
  case 'G':
    ioctl(fd, 3, atoi(argv[2]));
    break;
  case 'b':
  case 'B':
    ioctl(fd, 4, atoi(argv[2]));
    break;
  }
  
  close(fd);
  return 0;
}
