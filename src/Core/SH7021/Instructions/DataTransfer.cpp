#include "../Interpreter.h"
#include "SH7021.h"

INTERPRETER_INSTRUCTION(MOVA) {
    cpu->R[0] = (cpu->PC + 2) & 0xffff'fffc + (instruction.d.d << 2);
}