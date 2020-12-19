#include "SH7021.h"
#include "helpers.h"

CPU_INSTRUCTION(BRA) {
    if (DelayBranch()) {
        // PC needs to be 4 ahead anyway
        PC += (sext(instruction.d12.d, 12) << 1);
    }
}

CPU_INSTRUCTION(BSR) {
    if (DelayBranch()) {
        // PC needs to be 4 ahead anyway
        PR = PC;
        PC += (sext(instruction.d12.d, 12) << 1);;
    }
}

CPU_INSTRUCTION(JSR) {
    if (DelayBranch()) {
        // PC needs to be 4 ahead anyway
        PR = PC;
        PC = R[instruction.m.m];
    }
}

CPU_INSTRUCTION(RTS) {
    if (DelayBranch()) {
        // value of PC does not matter
        PC = PR;
    }
}

