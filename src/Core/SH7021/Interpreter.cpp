#include "Interpreter.h"

#include "SH7021.h"
#include "disasm.h"

INTERPRETER_INSTRUCTION(unimplemented) {
    s_OpcodeInfo instr = DisasmTable[instruction.raw];
    log_fatal("Unimplemented instruction %04x (%s %s) at %08x", instruction.raw, instr.mnemonic, instr.op_str, cpu->PC - 2);
}