#ifndef INLINED_INCLUDES

#include "../SH7021.h"

class SH7021INL : SH7021 {

#endif

template<bool S>
INSTRUCTION(MULxW) {
    if constexpr(S) {
        // signed
        MACL = (u32)((i16)R[instruction.nm.n] * (i16)R[instruction.nm.m]);
    }
    else {
        MACL = R[instruction.nm.n] * R[instruction.nm.m];
    }
}

template<typename T>
INSTRUCTION(EXTU) {
    R[instruction.nm.n] = (T)R[instruction.nm.m];
}

template<typename T>
INSTRUCTION(EXTS) {
    R[instruction.nm.n] = SignExtend<T>(R[instruction.nm.m]);
}

template<Condition C>
INSTRUCTION(CMP) {
    bool result;
    if constexpr(C == Condition::EQimm) {
        result = R[0] == SignExtend<u8>(instruction.i.i);
    }
    else {
        const u16 n = instruction.nm.n;
        const u16 m = instruction.nm.m;
        switch (C) {
            case Condition::EQ:
                result = R[n] == R[m];
                break;
            case Condition::GE:
                result = (i32)R[n] >= (i32)R[m];
                break;
            case Condition::GT:
                result = (i32)R[n] > (i32)R[m];
                break;
            case Condition::HI:
                result = R[n] >= R[m];
                break;
            case Condition::HS:
                result = R[n] >= R[m];
                break;
            case Condition::PL:
                result = (i32)R[n] > 0;
                break;
            case Condition::PZ:
                result = (i32)R[n] >= 0;
                break;
            case Condition::STR: {
                // interesting one!
                // pseudocode from the manual
                u32 temp = R[n] ^ R[m];
                u8 HH = temp >> 24;
                u8 HL = temp >> 16;
                u8 LH = temp >> 8;
                u8 LL = temp;
                result = !(HH && HL && LH && LL);
                break;
            }
        }
    }

    SR.T = result ? 1 : 0;
}


#ifndef INLINED_INCLUDES
};
#endif