#ifndef STACK_H
#define STACK_H

//unneccessary inclusion
#include <stdint.h>


/// Supported types: int/int-like types
typedef int StackUnit;
typedef int Stack_t;

/*
       0 is okay
    -1xx is genericStackError
    -2xx is genericFunctionExecutionError
*/
enum StackStatus {
    NaE                         =    0, //NotAnError
    InvalidStackID              = -100,
    NullDataPointerError        = -101,
    InvalidCapacityError        = -102,
    SizeOutOfBoundsError        = -103,
    CorruptedCanaryError        = -104,
    MemoryAllocationError       = -200,
    MemoryReallocationError     = -201,
    InvalidInitialCapacityError = -202,
    IncrementationError         = -203, // IncrementError
    TransmutedValueError        = -204,
    DecrementationError         = -205,
    FailedPopError              = -206, // Fail
    NothingToPopError           = -207
}; 
// size_of_elements -> n_elem
// size_of_apples -> n_apples
// of to in 
const uint32_t CONSTgenericStackError = 0x20;
const uint32_t CONSTgenericFunctionExecutionError = 0x40;

// enum StackError2 {
//     NaE                         =    0, //NotAnError
//     InvalidStackID              = CONSTgenericStackError,
//     NullDataPointerError        ,
//     InvalidCapacityError        ,
//     SizeOutOfBoundsError        ,
//     CorruptedCanaryError        ,
//     MemoryAllocationError       = CONSTgenericFunctionExecutionError,
//     MemoryReallocationError     ,
//     InvalidInitialCapacityError ,
//     IncrementationError         ,
//     TransmutedValueError        ,
//     DecrementationError         ,
//     FailedPopError              ,
//     NothingToPopError           
// };

Stack_t    stackInit(size_t initialCapacity);
StackStatus stackPush(Stack_t stk, StackUnit value);
StackUnit  stackPop(Stack_t stk, StackStatus* status = NULL);
StackStatus stackVerify(Stack_t stk);
StackStatus stackDestroy(Stack_t stk);

size_t stackGetSize(Stack_t stk, StackStatus* status = NULL);
size_t stackGetCapacity(Stack_t stk, StackStatus* status = NULL);
StackStatus stackGetError(Stack_t stk); //  

// ErrorDiscription {
//     StackStatus // popError
//     discription // size == 0
//     ling discription // на момент вызова функции pop  
//     // функции необходимо...
//     // ...
// }

void stackDump(FILE *fileStream, Stack_t stk,
               const char *fileName, int line);
#define stackDump(fileStream, stk) \
    stackDump(fileStream, stk, __FILE__, __LINE__);

#endif