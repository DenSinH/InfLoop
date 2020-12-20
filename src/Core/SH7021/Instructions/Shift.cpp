#include "SH7021.h"

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