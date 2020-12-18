#ifndef INLINED_INCLUDES

#include "../SH7021.h"

class SH7021INL : SH7021 {

#endif

template<typename T, AddressingMode src, AddressingMode dest>
INSTRUCTION(MOV) {
    const u8 m = instruction.nm.m;
    const u8 n = instruction.nm.n;

    log_debug("MOVx R%d, R%d", m, n);
    DoOperation<T, src, dest>(m, n, instruction.raw, [](u32 src_op, u32 dest_op){ return src_op; });
}

#ifndef INLINED_INCLUDES
};
#endif