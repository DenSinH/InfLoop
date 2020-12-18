#include "SH7021.h"

CPU_INSTRUCTION(MOVA) {
    R[0] = ((PC + 2) & 0xffff'fffc) + (instruction.d.d << 2);
}