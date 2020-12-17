#pragma once

#include "Interpreter.h"

typedef INSTRUCTION((*SH7021Instruction));

constexpr SH7021Instruction GetInstruction(u16 instruction) {
    switch (instruction & 0xff00) {
        case 0xc700:
            return &Interpreter::MOVA;
        default:
            break;
    }

    return &Interpreter::unimplemented;
}