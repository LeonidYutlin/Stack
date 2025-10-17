#ifndef STACK_SECURED_H
#define STACK_SECURED_H

#include "common/stackDef.h"
#include "common/dump/stackDump.h"

typedef int Stack_t;

#ifdef _DEBUG

#undef  stackDump
#define stackDump(fileStream, stk) \
        stackDumpSecured(fileStream, stk, __FILE__, __LINE__);
void stackDumpSecured(FILE* fileStream, Stack_t stk,
                      const char* fileName, int line);

#endif

Stack_t     stackInit(size_t initialCapacity, StackStatus* status = NULL);
StackStatus stackPush(Stack_t stk, StackUnit value);
StackUnit   stackPop(Stack_t stk, StackStatus* status = NULL);
StackStatus stackDestroy(Stack_t stk);
StackStatus stackVerify(Stack_t stk);
StackStatus stackExpandCapacity(Stack_t stk, size_t additionalCapacity);

size_t stackGetSize(Stack_t stk, StackStatus* status = NULL);
size_t stackGetCapacity(Stack_t stk, StackStatus* status = NULL);

//TODO Надо сделать лучше (в таком случае еще добавить к обычному стэку)
// StackStatus stackGetError(Stack_t stk);

// ErrorDiscription {
//     StackStatus // popError
//     discription // size == 0
//     ling discription // на момент вызова функции pop
//     // функции необходимо...
//     // ...
// }

#endif
