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

template<AddressingMode src, StatusRegister S>
INSTRUCTION(LDS) {
    // LDS/LDS.L instructions
    u32 value = GetSrcOperand<u32, src>(instruction.m.m, instruction.raw);
    switch (S) {
        case StatusRegister::PR:
            PR = value;
            break;
        default:
            log_fatal("Unimplemented status register for LDS/.L at PC = %08x", PC - 2);
    }
}

template<ControlRegister C, AddressingMode dest>
INSTRUCTION(STC) {
    // STC/STS.C instructions
    u32 value;
    switch (C) {
        case ControlRegister::SR:
            value = SR.raw;
            break;
        case ControlRegister::VBR:
            value = VBR;
            break;
        case ControlRegister::GBR:
            value = GBR;
            break;
        default:
            log_fatal("Invalid control register for LDC/.L at PC = %08x", PC - 2);
    }
    // never immediate, don't need second arg
    DoWriteback<u32, dest>(instruction.n.n, 0, PR);
}

template<StatusRegister S, AddressingMode dest>
INSTRUCTION(STS) {
    // STS/STS.L instructions
    u32 value;
    switch (S) {
        case StatusRegister::PR:
            // never immediate, don't need second arg
            value = PR;
            break;
        default:
            log_fatal("Unimplemented control register for STS/.L at PC = %08x", PC - 2);
    }
    DoWriteback<u32, dest>(instruction.n.n, 0, value);
}

#ifndef INLINED_INCLUDES
};
#endif
