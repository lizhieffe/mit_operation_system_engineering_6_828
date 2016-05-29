#ifndef UTIL_H_
#define UTIL_H_

#include <string.h>

// Simply print out the given func_name.
void PrintFuncName(const char* func_name);

// Iterates through the directories in $PATH to find given exe and returns its
// full path. If the exe cannot be found, returns the given exe.
char* SearchPath(char* exe);

#endif  // UTIL_H_ 
