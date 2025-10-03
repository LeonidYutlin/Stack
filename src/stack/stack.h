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

#ifndef STACK_POISON
#define STACK_POISON 0xAbD06
#endif

#ifndef CANARY_LEFT_COUNT
#define CANARY_LEFT 1
#endif

#ifndef CANARY_RIGHT_COUNT
#define CANARY_RIGHT 1
#endif

#ifndef CANARY_LEFT
#define CANARY_LEFT 0xDEDDEDED
#endif

#ifndef CANARY_RIGHT
#define CANARY_RIGHT 0xDEDEDDED
#endif

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

typedef struct Stack {
    StackUnit* data = NULL;
    size_t size = 0;
    size_t capacity = 0;
    StackError error = UninitializedStackError;
    bool isDestroyed = false;
    bool isInitialized = false;
    //char state = '\0' 'd' 'i' 'e'
} Stack;

static const char* QUOTES[] = {
    "Canary Canary Do Thy Hear Me ?",
    "SegFault? Not on my watch, bucko",
    "Aaaaand what has went wrong now?",
    "None of these words are in the Bible",
    "Et tu, Brute?",
    "Appledog",
    "Is my stack gonna make it? Prolly not",
    "OUR FOOD KEEPS BLOWING U-",
    "Now with 800% more poison!",
    "One must imagine stackDump happy...",
    "Boy oh boy I sure hope nothing bad has happened to my stack!",
    "I ATE IT ALL",
    "Ouch! That hurt!",
    "stackDump soboleznuet",
    "Tut mogla bit' vasha oshibka",
    "So what are you gonna say at my funeral now that you've killed me"
};

StackError stackInit(Stack* stk, size_t initialCapacity);
StackError stackPush(Stack* stk, StackUnit value);
StackUnit stackPop(Stack* stk, StackError* error = NULL);
StackError stackVerify(Stack* stk);
StackError stackDestroy(Stack* stk);

void stackDump(FILE* fileStream, Stack* stk, const char* fileName, int line);
#define stackDump(fileStream, stk) stackDump(fileStream, stk, __FILE__, __LINE__);

#endif