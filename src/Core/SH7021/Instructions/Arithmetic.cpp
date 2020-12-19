#include "../SH7021.h"

CPU_INSTRUCTION(ADD) {
    R[instruction.nm.n] += R[instruction.nm.m];
}

CPU_INSTRUCTION(ADDI) {
    R[instruction.ni.n] += SignExtend<u8>(instruction.ni.i);
}