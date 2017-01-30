#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

int main() {
  int p[2];
  char *argv[2];

  argv[0] = "wc";
  argv[1] = 0;

  pipe(p);

  if (fork() == 0) {
    close(0);
    int fd = dup(p[0]);
    std::cout << "new fd is " << fd << std::endl;
    close(p[0]);
    close(p[1]);
    execv("/usr/bin/wc", argv);
  } else {
    close(p[0]);
    write(p[1], "hello world hahaha\n", 19);
    close(p[1]);
  }
}
