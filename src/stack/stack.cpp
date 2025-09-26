#include <stddef.h>
#include <stdbool.h>

typedef struct Stack {
    int* data = NULL;
    size_t size = 0;
    size_t capacity = 0;
    bool isDestroyed = false;
    bool isInitialized = false;
} Stack;

enum StackError