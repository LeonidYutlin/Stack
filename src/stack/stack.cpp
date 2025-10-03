#include "stack.h"
#undef stackDump

void stackDump(FILE *fileStream, Stack *stk,
               const char *fileName, int line){
    assert(fileStream);

    static long callCount = 0;

    static bool isRandomInitialized = false;
    if (!isRandomInitialized) {
        srand((unsigned int)time(NULL));
        isRandomInitialized = true;
    }

    StackError stackError = stackVerify(stk);
    if (stackError == NullStackPointerError) {
        fprintf(fileStream,
                "-----------------------\n"
                "[Q] %s\n"
                "stackDump #%ld called from %s:%d\n"
                "Stack [NULL] {}\n"
                "-----------------------\n",
                QUOTES[(unsigned long)random()
                        % (sizeof(QUOTES) / sizeof(char *))],
                callCount++, fileName, line);
    } else {
        fprintf(fileStream,
                "-----------------------\n"
                "[Q] %s\n"
                "stackDump #%ld called from %s:%d\n"
                "Stack [%p]\n"
                "{\n"
                "\tsize     = %lu\n"
                "\tcapacity = %lu\n"
                "\t%serror = %d\n"
                "\tisInitialized = %d\n"
                "\tisDestroyed   = %d\n",
                QUOTES[(unsigned long)random()
                        % (sizeof(QUOTES) / sizeof(char *))],
                callCount++, fileName, line,
                stk,
                stk->size,
                stk->capacity,
                stk->error == NaE ? "" : "[!] ", stk->error,
                stk->isInitialized,
                stk->isDestroyed);

        if (stackError == NullStackPointerError) {
            fprintf(fileStream,
                    "\tdata [NULL] {}\n"
                    "}\n"
                    "-----------------------\n");
        }
        else {
            fprintf(fileStream,
                    "\tdata [%p]\n"
                    "\t{\n",
                    stk->data);
            for (size_t i = 0; i < stk->capacity; i++) {
                StackUnit value = stk->data[i];
                bool isPoison = value == STACK_POISON;
                bool isAsterisked = isPoison || i >= stk->size;
                fprintf(fileStream,
                        "\t\t%c[%lu] = %d%s\n",
                        isAsterisked ? ' ' : '*',
                        i, value,
                        isPoison ? "(POISON)" : "");
            }
            fprintf(fileStream,
                    "\t}\n"
                    "}\n"
                    "-----------------------\n");
        }
    }
}

StackError stackInit(Stack *stk, size_t initialCapacity){
    StackError stackError = NaE;
    if ((stackError = stackVerify(stk)) && stackError != UninitializedStackError)
        return stackError;

    if (stk->isInitialized)
        return stk->error = ReinitializationError;

    StackUnit *tempPtr = (StackUnit*)calloc(initialCapacity, sizeof(StackUnit));
    if (!tempPtr)
        return stk->error = MemoryAllocationError;

    stk->data = tempPtr;
    stk->size = 0;
    stk->capacity = initialCapacity;
    stk->error = NaE;
    stk->isDestroyed = false;
    stk->isInitialized = true;

    /*
    A way of doing all this via cpp's default struct values
    *stk = {.data = tempPtr,
            .capacity = initialCapacity,
            .isInitialized = true
            .error = NaE};
    */

    /*
    if (!(stackError = stackVerify(stk)))
        return stackError;

    return stackError;
    */

    return stackVerify(stk);
}

StackError stackPush(Stack *stk, StackUnit value){
    StackError stackError = NaE;
    if ((stackError = stackVerify(stk)))
        return stackError;

    size_t previousSize = stk->size;
    if (previousSize + 1 == stk->capacity) {
        StackUnit *tempPtr = (StackUnit*)realloc(stk->data,
                                                 2 * stk->capacity 
                                                 * sizeof(StackUnit));
        if (!tempPtr)
            return stk->error = MemoryReallocationError;
        
        memset(tempPtr + previousSize + 1, 0, 
               stk->capacity * sizeof(StackUnit));
        stk->capacity *= 2;
        stk->data = tempPtr;
    }

    stk->data[stk->size++] = value;

    if (stk->size != previousSize + 1)
        return stk->error = IncrementationError;
    if (stk->data[previousSize] != value)
        return stk->error = TransmutedValueError;

    return stackVerify(stk);
}

StackUnit stackPop(Stack *stk, StackError *error){
    StackError stackError = NaE;
    if ((stackError = stackVerify(stk)))
    {
        if (error)
            *error = stk->error;
        return STACK_POISON;
    }

    size_t previousSize = stk->size;

    if (stk->size == 0)
    {
        stk->error = EmptyStackDataError;
        if (error)
            *error = stk->error;
        return STACK_POISON;
    }

    StackUnit value = stk->data[--stk->size];
    stk->data[stk->size] = 0;

    if (stk->size != previousSize - 1)
    {
        stk->error = DecrementationError;
        if (error)
            *error = stk->error;
        return value;
    }
    if (stk->data[previousSize] != 0)
    {
        stk->error = FailedPopError;
        if (error)
            *error = stk->error;
        return value;
    }

    if (error)
        *error = stackVerify(stk);
    return value;
}

StackError stackDestroy(Stack* stk){
    StackError stackError = NaE;
    stackError = stackVerify(stk);
    switch (stackError) {
        case NaE:
        case NullDataPointerError:
        case InvalidCapacityError:
        case SizeOutOfBoundsError: break;
        default:
            return stackError;
    }

    if (*stk->data) {
        for (size_t i = 0; i < stk->capacity; i++)
            stk->data[i] = STACK_POISON;
        free(stk->data);
        stk->data = NULL;
    }

    stk->size = 0;
    stk->capacity = 0;
    stk->error = DestroyedStackError;
    stk->isInitialized = false;
    stk->isDestroyed = true;

    //free(stk);

    return NaE;
}

StackError stackVerify(Stack *stk){
    if (!stk)
        return NullStackPointerError;
    if (stk->isDestroyed)
        return stk->error = DestroyedStackError;
    if (!stk->isInitialized)
        return stk->error = UninitializedStackError;
    if (!stk->data)
        return stk->error = NullDataPointerError;
    if (stk->capacity == 0)
        return stk->error = InvalidCapacityError;
    if (stk->size >= stk->capacity)
        return stk->error = SizeOutOfBoundsError;
    return stk->error = NaE;
}