#include "SH7021.h"

#include "Tables.h"
#include "disasm.h"

std::array<SH7021Instruction, 0x10000> SH7021::instructions = [] {
    std::array<SH7021Instruction, 0x10000> table = {};

    for (size_t i = 0; i < 0x10000; i++) {
        table[i] = GetInstruction(i);
    }

    return table;
}();

SH7021::SH7021(Memory* Mem, i32* timer) {
    this->Mem = Mem;
    this->timer = timer;
}

CPU_INSTRUCTION(unimplemented) {
    s_OpcodeInfo instr = DisasmTable[instruction.raw];
    log_fatal("Unimplemented instruction %04x (%s %s) at %08x", instruction.raw, instr.mnemonic, instr.op_str, PC - 2);
}


void SH7021::Step() {
    u16 opcode = Mem->Read<u16>(PC);
    PC += 2;

    (this->*SH7021::instructions[opcode])(s_instruction{.raw = opcode});
    (*timer)++;
}
