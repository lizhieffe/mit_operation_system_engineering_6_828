#include <errno.h>
#include <iostream>
#include <unistd.h>
#include <stdio.h>

int main() {
  char buf[512];
  int read_status;
  int write_status;

  for (;;) {
    read_status = read(0, buf, sizeof buf);
    std::cout << "trying to read buf with size = " << sizeof buf << std::endl;
    if (read_status == 0) {
      std::cout << "all buf are read." << std::endl;
      break;
    } else if (read_status < 0) {
      perror("read()");
      exit(1);
    }

    write_status = write(1, buf, read_status);
    if (read_status != write_status) {
      std::cerr << "write error." << std::endl;
    }
  }

  return 0;
}
