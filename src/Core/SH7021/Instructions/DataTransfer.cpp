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