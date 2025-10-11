#ifndef STACK_STATUS_H
#define STACK_STATUS_H

#include <stdint.h>

/// Supported types: int/int-like types
typedef int StackUnit;

static const uint8_t CONSTgenericStackError                  = 0x10;
static const uint8_t CONSTgenericStackFunctionExecutionError = 0x20;
static const uint8_t CONSTgenericStackManagerError           = 0x30;

enum StackStatus {
    OK = 0,
    UninitializedStack = CONSTgenericStackError,
    DestroyedStack,
    NullStackPointer,
    NullDataPointer,
    BadCapacity,
    BadSize,
    CorruptedCanary,
    AttemptedReinitialization = CONSTgenericStackFunctionExecutionError,
    FailMemoryAllocation,
    FailMemoryReallocation,
    BadInitialCapacity,
    FailIncrement,
    PushedValueTransmuted,
    FailDecrement,
    FailPop, //FauxPop (i like FauxPop a lot)
    NoValueToPop,
    InvalidStackID = CONSTgenericStackManagerError
};

#endif