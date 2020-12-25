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

CPU_INSTRUCTION(ADDC) {
    u32 op1 = R[instruction.nm.n];
    u32 op2 = R[instruction.nm.m];
    u32 carry_in = SR.T ? 1 : 0;
    u32 result = op1 + op2 + carry_in;
    R[instruction.nm.n] = result;

    SR.T = (((u64)op1 + (u64)op2 + (u64)carry_in) > 0xffff'ffffULL) ? 1 : 0;
}

CPU_INSTRUCTION(SUB) {
    R[instruction.nm.n] -= R[instruction.nm.m];
}

CPU_INSTRUCTION(SUBC) {
    u32 op1 = R[instruction.nm.n];
    u32 op2 = R[instruction.nm.m];
    u32 carry_in = SR.T ? 1 : 0;
    u32 result = op1 - op2 - carry_in;
    R[instruction.nm.n] = result;

    SR.T = ((op2 + carry_in) <= op1) ? 1 : 0;
}

CPU_INSTRUCTION(NEG) {
    R[instruction.nm.n] = -R[instruction.nm.m];
}

CPU_INSTRUCTION(NEGC) {
    u32 temp = -R[instruction.nm.m];
    R[instruction.nm.n] = -R[instruction.nm.m] - (SR.T ? 1 : 0);
    SR.T = (temp != 0 || (temp < R[instruction.nm.n])) ? 1 : 0;
}

CPU_INSTRUCTION(DIV0U) {
    SR.M = 0;
    SR.Q = 0;
    SR.T = 0;
}

CPU_INSTRUCTION(DIV0S) {
    const u16 n = instruction.nm.n;
    const u16 m = instruction.nm.m;

    SR.Q = ((i32)R[n] < 0) ? 1 : 0;
    SR.M = ((i32)R[m] < 0) ? 1 : 0;
    SR.T = (SR.M != SR.Q) ? 1 : 0;
}

CPU_INSTRUCTION(DIV1) {
    const u16 m = instruction.nm.m;
    const u16 n = instruction.nm.n;

    // this is literally the pseudocode from the manual
    // this instruction seems extremely wacky, but I guess this works
    u32 tmp0;
    bool old_q, tmp1;

    old_q = SR.Q != 0;
    SR.Q = (R[n] & 0x8000'0000) ? 1 : 0;
    R[n] <<= 1;
    R[n] |= SR.T ? 1 : 0;
    if (!old_q) {
        if (!SR.M) {
            tmp0 = R[n];
            R[n] -= R[m];
            tmp1 = R[n] > tmp0;
            if (!SR.Q) {
                SR.Q = tmp1 ? 1 : 0;
            }
            else {
                SR.Q = tmp1 ? 0 : 1;
            }
        }
        else {
            tmp0 = R[n];
            R[n] += R[m];
            tmp1 = R[n] < tmp0;
            if (!SR.Q) {
                SR.Q = tmp1 ? 0 : 1;
            }
            else {
                SR.Q = tmp1 ? 1 : 0;
            }
        }
    }
    else {
        if (!SR.M) {
            tmp0 = R[n];
            R[n] += R[m];
            tmp1 = R[n] < tmp0;
            if (!SR.Q) {
                SR.Q = tmp1 ? 1 : 0;
            }
            else {
                SR.Q = tmp1 ? 0 : 1;
            }
        }
        else {
            tmp0 = R[n];
            R[n] -= R[m];
            tmp1 = R[n] > tmp0;
            if (!SR.Q) {
                SR.Q = tmp1 ? 0 : 1;
            }
            else {
                SR.Q = tmp1 ? 1 : 0;
            }
        }
    }
    SR.T = (SR.Q == SR.M) ? 1 : 0;
}