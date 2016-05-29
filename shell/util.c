#include "util.h"

#include <stdio.h>

void PrintFuncName(const char* func_name) {
  fprintf(stdout, "%s: is called.\n", func_name);
}
