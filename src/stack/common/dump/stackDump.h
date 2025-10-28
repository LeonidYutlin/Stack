#ifndef STACK_DUMP_H
#define STACK_DUMP_H

#ifdef _DEBUG

#include <stdio.h>
#include "../../stack.h"

void stackDump(FILE* fileStream, Stack* stk, bool isAdvanced,
               const char* fileName, int line);
#define stackDump(fileStream, stk) \
        stackDump(fileStream, stk, false, __FILE__, __LINE__);

#else

#define stackDump(fileStream, stk) ;

#endif

#endif
