#ifndef STACK_CONST_H
#define STACK_CONST_H

#include <stddef.h>
#include "stackDef.h"

static const StackUnit STACK_POISON = 0xAbD06;

static const StackUnit CANARY_LEFT  = (StackUnit)0xDEDDEDED;
static const StackUnit CANARY_RIGHT = (StackUnit)0xAAEDAEDA;

static const size_t CANARY_LEFT_COUNT  = 1;
static const size_t CANARY_RIGHT_COUNT = 1;

#endif