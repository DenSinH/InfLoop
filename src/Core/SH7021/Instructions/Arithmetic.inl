#ifndef INLINED_INCLUDES

#include "../SH7021.h"

class SH7021INL : SH7021 {

#endif

template<typename T>
INSTRUCTION(EXTU) {
    R[instruction.nm.n] = (T)R[instruction.nm.m];
}

template<typename T>
INSTRUCTION(EXTS) {
    R[instruction.nm.n] = SignExtend<T>(R[instruction.nm.m]);
}


#ifndef INLINED_INCLUDES
};
#endif