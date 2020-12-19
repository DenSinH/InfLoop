#include "SH7021.h"

CPU_INSTRUCTION(BRA) {
    if (unlikely(InBranchDelay)) {
        // invalid branch delay slot
        return;
    }

    // signify we are in a branch delay to prevent certain instructions (mainly branches) from happening
    InBranchDelay = true;
    Step();
    InBranchDelay = false;

    // PC needs to be 4 ahead anyway
    PC += (instruction.d12.d << 1);
}

CPU_INSTRUCTION(JSR) {
    if (unlikely(InBranchDelay)) {
        // invalid branch delay slot
        return;
    }

    InBranchDelay = true;
    Step();
    InBranchDelay = false;

    PR = PC; // needs to be 4 bytes ahead anyway
    PC = R[instruction.m.m];
}

