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

