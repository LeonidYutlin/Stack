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
    -1xx is genericError
    -2xx is initSpecificError
    -3xx is pushSpecificError
    -9xx is otherError
*/
enum StackError {
    NaE                         =    0, //NotAnError
    NullStackPointerError       = -100,
    NullDataPointerError        = -101,
    DestroyedStackError         = -102,
    UninitializedStackError     = -103,
    InvalidCapacityError        = -104,
    SizeOutOfBoundsError        = -105,
    CorruptedCanaryError        = -106,
    InvalidStackID              = -107,
    ReinitializationError       = -200,
    MemoryAllocationError       = -201,
    MemoryReallocationError     = -202,
    InvalidInitialCapacityError = -203,
    IncrementationError         = -300,
    TransmutedValueError        = -301,
    DecrementationError         = -400,
    FailedPopError              = -401,
    EmptyStackDataError         = -402,
    UnidentifiedStackError      = -900
};

int stackInit(size_t initialCapacity);
StackError stackPush(int stkID, StackUnit value);
StackUnit stackPop(int stkID, StackError* error = NULL);
StackError stackVerify(int stkID);
StackError stackDestroy(int stkID);

size_t stackGetSize(int stkID, StackError* error = NULL);
size_t stackGetCapacity(int stkID, StackError* error = NULL);
StackError stackGetError(int stkID);

void stackDump(FILE* fileStream, int stkID, const char* fileName, int line);
#define stackDump(fileStream, stkID) stackDump(fileStream, stkID, __FILE__, __LINE__);

#endif