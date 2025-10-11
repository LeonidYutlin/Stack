#ifndef STACK_H
#define STACK_H

#include <stdint.h>

/// Supported types: int/int-like types
typedef int StackUnit;

const uint8_t CONSTgenericStackError             = 0x10;
const uint8_t CONSTgenericFunctionExecutionError = 0x20;

enum StackStatus {
    OK = 0,
    UninitializedStack = CONSTgenericStackError,
    DestroyedStack,
    NullStackPointer,
    NullDataPointer,
    BadCapacity,
    BadSize,
    CorruptedCanary,
    AttemptedReinitialization = CONSTgenericFunctionExecutionError,
    FailMemoryAllocation,
    FailMemoryReallocation,
    BadInitialCapacity,
    FailIncrement,
    PushedValueTransmuted,
    FailDecrement,
    FailPop, //FauxPop (i like FauxPop a lot)
    NoValueToPop
};

typedef struct Stack {
    StackUnit* data = NULL;
    size_t size = 0;
    size_t capacity = 0;
    size_t capacityWithCanaries = 0;
    StackStatus status = UninitializedStack;
} Stack;

StackStatus stackInit(Stack* stk, size_t initialCapacity);
Stack* stackInit(size_t initialCapacity, StackStatus* status = NULL);
StackStatus stackPush(Stack* stk, StackUnit value);
StackUnit stackPop(Stack* stk, StackStatus* status = NULL);
StackStatus stackVerify(Stack* stk);
StackStatus stackDestroy(Stack* stk, bool isAlloced = false);
StackStatus stackExpandCapacity(Stack* stk, size_t additionalCapacity);

#ifdef _DEBUG

void stackDump(FILE* fileStream, Stack* stk, const char* fileName, int line);
#define stackDump(fileStream, stk) stackDump(fileStream, stk, __FILE__, __LINE__);

#else 

#define stackDump(fileStream, stk) ;

#endif

#endif