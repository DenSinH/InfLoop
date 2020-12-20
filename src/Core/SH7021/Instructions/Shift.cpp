#include "SH7021.h"

#include "helpers.h"

CPU_INSTRUCTION(SHLL) {
    const u16 n = instruction.n.n;
    SR.T = (R[n] & 0x8000'0000) ? 1 : 0;
    R[n] <<= 1;
}

CPU_INSTRUCTION(SHLR) {
    const u16 n = instruction.n.n;
    SR.T = (R[n] & 1) ? 1 : 0;
    R[n] >>= 1;
}

CPU_INSTRUCTION(SHAR) {
    const u16 n = instruction.n.n;
    SR.T = (R[n] & 1) ? 1 : 0;
    R[n] = ((i32)R[n]) >> 1;
}

CPU_INSTRUCTION(ROTL) {
    const u16 n = instruction.n.n;
    SR.T = (R[n] & 0x8000'0000) ? 1 : 0;
    R[n] = ROTL32(R[n], 1);
}

CPU_INSTRUCTION(ROTR) {
    const u16 n = instruction.n.n;
    SR.T = (R[n] & 1) ? 1 : 0;
    R[n] = ROTR32(R[n], 1);
}

CPU_INSTRUCTION(ROTCL) {
    const u16 n = instruction.n.n;
    u32 old_t = SR.T ? 1 : 0;
    SR.T = (R[n] & 0x8000'0000) ? 1 : 0;
    R[n] = (R[n] << 1) | old_t;
}

CPU_INSTRUCTION(ROTCR) {
    const u16 n = instruction.n.n;
    u32 old_t = SR.T ? 0x8000'0000 : 0;
    SR.T = (R[n] & 1) ? 1 : 0;
    R[n] = (R[n] >> 1) | old_t;
}