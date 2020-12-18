#ifndef INLINED_INCLUDES

#include "../SH7021.h"

class SH7021INL : SH7021 {

#endif

template<AddressingMode src, ControlRegister C>
INSTRUCTION(LDC) {
    // LDC/LDC.L instructions
    u32 value = GetSrcOperand<u32, src>(instruction.m.m, instruction.raw);
    switch (C) {
        case ControlRegister::SR:
            SR.raw = value;
            break;
        case ControlRegister::VBR:
            VBR = value;
            break;
        case ControlRegister::GBR:
            GBR = value;
            break;
        default:
            log_fatal("Invalid control register for LDC/.L at PC = %08x", PC - 2);
    }
}

#ifndef INLINED_INCLUDES
};
#endif
