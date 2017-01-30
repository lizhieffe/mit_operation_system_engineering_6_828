#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

int main() {
  char *argv[2];
  argv[0] = "cat";
  argv[1] = 0;
  if (fork() == 0) {
    close(0);
    int fd = open("../input.txt",  O_RDONLY);
    std::cout << "fd = " << fd << std::endl;
    execv("/bin/cat", argv);
  }
}
