#pragma once

#include "Interpreter.h"

typedef INSTRUCTION((*SH7021Instruction));

constexpr SH7021Instruction GetInstruction(u16 instruction) {
    return &Interpreter::unimplemented;
}