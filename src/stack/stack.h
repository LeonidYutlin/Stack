#ifndef STACK_H
#define STACK_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

typedef int StackUnit;

/*
       0 is okay
    -1xx is genericStackError
    -2xx is genericFunctionExecutionError
*/
enum StackError {
    NaE                         =    0, //NotAnError
    InvalidStackID              = -100,
    NullDataPointerError        = -101,
    InvalidCapacityError        = -102,
    SizeOutOfBoundsError        = -103,
    CorruptedCanaryError        = -104,
    MemoryAllocationError       = -200,
    MemoryReallocationError     = -201,
    InvalidInitialCapacityError = -202,
    IncrementationError         = -203,
    TransmutedValueError        = -204,
    DecrementationError         = -205,
    FailedPopError              = -206,
    EmptyStackDataError         = -207
};

int stackInit(size_t initialCapacity);
StackError stackPush(int stkID, StackUnit value);
StackUnit stackPop(int stkID, StackError* error = NULL);
StackError stackVerify(int stkID);
StackError stackDestroy(int stkID);

size_t stackGetSize(int stkID, StackError* error = NULL);
size_t stackGetCapacity(int stkID, StackError* error = NULL);
StackError stackGetError(int stkID);

void stackDump(FILE *fileStream, int stkID,
               const char *fileName, int line);
#define stackDump(fileStream, stkID) \
    stackDump(fileStream, stkID, __FILE__, __LINE__);

#endif