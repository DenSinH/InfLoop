#ifndef INLINED_INCLUDES

#include "../SH7021.h"

class SH7021INL : SH7021 {

#endif

template<u8 n>
INSTRUCTION(SHLLn) {
    R[instruction.n.n] <<= n;
}

template<u8 n>
INSTRUCTION(SHLRn) {
    R[instruction.n.n] >>= n;
}

#ifndef INLINED_INCLUDES
};
#endif