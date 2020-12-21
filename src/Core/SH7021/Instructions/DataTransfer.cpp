#include "SH7021.h"

CPU_INSTRUCTION(MOVA) {
    R[0] = ((PC + 2) & 0xffff'fffc) + (instruction.d.d << 2);
}

CPU_INSTRUCTION(MOVIimm) {
    R[instruction.ni.n] = SignExtend<u8>(instruction.ni.i);
}

CPU_INSTRUCTION(MOVT) {
    R[instruction.n.n] = SR.T ? 1 : 0;
}

CPU_INSTRUCTION(SWAPB) {
    const u16 n = instruction.nm.n;
    const u16 m = instruction.nm.m;

    u32 hiword = R[m] & 0xffff'0000;
    R[n] = hiword | bswap_16((u16)R[m]);
}

CPU_INSTRUCTION(SWAPW) {
    const u16 n = instruction.nm.n;
    const u16 m = instruction.nm.m;

    R[n] = (R[m] << 16) | (R[m] >> 16);
}

CPU_INSTRUCTION(XTRCT) {
    const u16 n = instruction.nm.n;
    const u16 m = instruction.nm.m;

    R[n] = (R[m] << 16) | (R[n] >> 16);
}
