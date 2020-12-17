#pragma once

#include "default.h"
#include "log.h"

#include "Instruction.h"

class SH7021;

#define INSTRUCTION(_name) void _name(SH7021* cpu, s_instruction instruction)
#define STATIC_INSTRUCTION(_name) static INSTRUCTION(_name)
#define INTERPRETER_INSTRUCTION(_name) INSTRUCTION(Interpreter::_name)

class Interpreter {
public:
    STATIC_INSTRUCTION(unimplemented);

    STATIC_INSTRUCTION(MOVA);
};