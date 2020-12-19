#ifndef INLINED_INCLUDES

#include "../SH7021.h"

class SH7021INL : SH7021 {

#endif

template<bool T>
INSTRUCTION(Bt) {
    if (T == (SR.T == 1)) {
        PC += 2; // should be 4 ahead, is 2 ahead
        PC += SignExtend<u8>(instruction.d.d) << 1;
    }
}


#ifndef INLINED_INCLUDES
};
#endif