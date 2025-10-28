#ifndef STACK_H
#define STACK_H

#include <stddef.h>
#include "common/stackDef.h"

typedef struct Stack {
    StackUnit* data = NULL;
    size_t size                 = 0;
    size_t capacity             = 0;
    size_t capacityWithCanaries = 0;
    StackStatus status = UninitializedStack;
} Stack;

#include "common/dump/stackDump.h"

StackStatus stackInit(Stack* stk, size_t initialCapacity);
// другое имя... мне это не прям нравится но оверлоадить я так сильно не хочу
Stack*      stackDynamicInit(size_t initialCapacity, StackStatus* status = NULL);
StackStatus stackPush(Stack* stk, StackUnit value);
StackUnit   stackPop(Stack* stk, StackStatus* status = NULL);
StackStatus stackDestroy(Stack* stk, bool isAlloced = false);
StackStatus stackVerify(Stack* stk);
StackStatus stackExpandCapacity(Stack* stk, size_t additionalCapacity);

#endif
