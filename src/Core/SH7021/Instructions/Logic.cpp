#include "../SH7021.h"

CPU_INSTRUCTION(TST) {
    const u8 n = instruction.nm.n;
    const u8 m = instruction.nm.m;

    SR.T = 0;
    if ((R[n] & R[m]) == 0) {
        SR.T = 1;
    }
}

CPU_INSTRUCTION(TSTI) {
    const u8 imm = instruction.i.i;

    SR.T = 0;
    if ((R[0] & imm) == 0) {
        SR.T = 1;
    }
}

CPU_INSTRUCTION(TSTB) {
    u16 op2 = GetSrcOperand<u8, AddressingMode::IndirectIndexedGBR>(0, 0);

    SR.T = 0;
    if ((instruction.i.i & op2) == 0) {
        SR.T = 1;
    }
}

CPU_INSTRUCTION(AND) {
    const u8 n = instruction.nm.n;
    const u8 m = instruction.nm.m;

    R[n] &= R[m];
}

CPU_INSTRUCTION(ANDI) {
    R[0] &= instruction.i.i;
}

CPU_INSTRUCTION(ANDB) {
    u16 op2 = GetSrcOperand<u8, AddressingMode::IndirectIndexedGBR>(0, 0);

    DoWriteback<u8, AddressingMode::IndirectIndexedGBR>(0, 0, instruction.i.i & op2);
}