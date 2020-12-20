#ifndef INLINED_INCLUDES

#include "../SH7021.h"

class SH7021INL : SH7021 {

#endif

template<typename T, AddressingMode src, AddressingMode dest>
INSTRUCTION(MOV) {
    const u8 m = instruction.nm.m;
    const u8 n = instruction.nm.n;

    DoOperation<T, src, dest, false>(m, n, instruction.raw, [](u32 src_op, u32 dest_op){ return src_op; });
}

template<typename T>
INSTRUCTION(MOVI) {
    const u8 n = instruction.nd8.n;

    DoOperation<T, AddressingMode::PCRelativeD8, AddressingMode::DirectRegister, false>(
            0, n, instruction.raw, [](u32 src_op, u32 dest_op){ return src_op; }
    );
}

template<typename T, bool from_gbr>
INSTRUCTION(MOVgbrdisp) {
    // MOV.x @(disp, GBR), R0 OR MOV.x R0, @(disp, GBR)
    if constexpr(from_gbr) {
        DoOperation<T, AddressingMode::IndirectGBRDisplacement, AddressingMode::DirectRegister, false>(
                0, 0, instruction.raw, [](u32 src_op, u32 dest_op){ return src_op; }
        );
    }
    else {
        DoOperation<T, AddressingMode::DirectRegister, AddressingMode::IndirectGBRDisplacement, false>(
                0, 0, instruction.raw, [](u32 src_op, u32 dest_op){ return src_op; }
        );
    }
}

template<typename T, bool from_disp>
INSTRUCTION(MOVregdisp) {
    // MOV.x @(disp, GBR), R0 OR MOV.x R0, @(disp, GBR)
    u16 m;
    u16 n;

    if constexpr(from_disp) {
        if constexpr(std::is_same_v<T, u32>) {
            m = instruction.nm.m;
            n = instruction.nm.n;
        }
        else {
            m = instruction.nm.m;
            n = 0;
        }

        DoOperation<T, AddressingMode::IndirectRegisterDisplacement, AddressingMode::DirectRegister, false>(
                m, n, instruction.raw, [](u32 src_op, u32 dest_op){ return src_op; }
        );
    }
    else {
        if constexpr(std::is_same_v<T, u32>) {
            m = instruction.nm.m;
            n = instruction.nm.n;
        }
        else {
            m = 0;
            n = instruction.nm.m;
        }

        DoOperation<T, AddressingMode::DirectRegister, AddressingMode::IndirectRegisterDisplacement, false>(
                m, n, instruction.raw, [](u32 src_op, u32 dest_op){ return src_op; }
        );
    }
}

#ifndef INLINED_INCLUDES
};
#endif