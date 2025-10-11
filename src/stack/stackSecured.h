#ifndef STACK_SECURED_H
#define STACK_SECURED_H

#include "common/stackDef.h"

typedef int Stack_t;

Stack_t     stackInit(size_t initialCapacity);
StackStatus stackPush(Stack_t stk, StackUnit value);
StackUnit   stackPop(Stack_t stk, StackStatus* status = NULL);
StackStatus stackVerify(Stack_t stk);
StackStatus stackDestroy(Stack_t stk);

size_t stackGetSize(Stack_t stk, StackStatus* status = NULL);
size_t stackGetCapacity(Stack_t stk, StackStatus* status = NULL);
StackStatus stackGetError(Stack_t stk);

// ErrorDiscription {
//     StackStatus // popError
//     discription // size == 0
//     ling discription // на момент вызова функции pop  
//     // функции необходимо...
//     // ...
// }

#ifdef _DEBUG

void stackDump(FILE* fileStream, Stack_t stk, const char* fileName, int line);
#define stackDump(fileStream, stk) stackDump(fileStream, stk, __FILE__, __LINE__);

#else 

#define stackDump(fileStream, stk) ;

#endif

#endif