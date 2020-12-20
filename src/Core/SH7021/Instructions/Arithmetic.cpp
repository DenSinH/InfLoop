#include "../SH7021.h"

CPU_INSTRUCTION(ADD) {
    R[instruction.nm.n] += R[instruction.nm.m];
}

CPU_INSTRUCTION(ADDI) {
    R[instruction.ni.n] += SignExtend<u8>(instruction.ni.i);
}

CPU_INSTRUCTION(ADDV) {
    u32 op1 = R[instruction.nm.n];
    u32 op2 = R[instruction.nm.m];
    u32 result = op1 + op2;
    R[instruction.nm.n] = result;

    SR.T = (((op1 ^ result) & (~op1 ^ op2)) >> 31) ? 1 : 0;
}

CPU_INSTRUCTION(SUB) {
    R[instruction.nm.n] -= R[instruction.nm.m];
}