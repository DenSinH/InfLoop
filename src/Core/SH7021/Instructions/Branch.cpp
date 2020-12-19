#include "SH7021.h"

CPU_INSTRUCTION(BRA) {
    if (DelayBranch()) {
        // PC needs to be 4 ahead anyway
        PC += (instruction.d12.d << 1);
    }
}

CPU_INSTRUCTION(BSR) {
    if (DelayBranch()) {
        // PC needs to be 4 ahead anyway
        PR = PC;
        PC += (instruction.d12.d << 1);;
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

