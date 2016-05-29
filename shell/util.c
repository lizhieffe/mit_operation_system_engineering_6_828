#include "util.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void PrintFuncName(const char* func_name) {
  fprintf(stdout, "%s: is called.\n", func_name);
}

char* SearchPath(char* exe) {
  DIR* d;
  struct dirent* dir;

  char* paths = getenv("PATH");
  char* path_dir = strtok(paths, ":");
  while (path_dir != NULL) {
    d = opendir(path_dir);
    if (d == NULL) {
      fprintf(stderr, "Cannot open dir: %s", strerror(errno));
    } else {
      while ((dir = readdir(d)) != NULL) {
        if (strcmp(dir->d_name, exe) == 0) {
          char* final_path = malloc(strlen(path_dir) + strlen(exe) + 2);
          final_path = strcat(final_path, path_dir);
          final_path = strcat(final_path, "/");
          final_path = strcat(final_path, exe);
          return final_path;
        }
      }
    }
    path_dir = strtok(NULL, ":");
  }
  return exe;
}
