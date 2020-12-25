#include "../SH7021.h"

CPU_INSTRUCTION(CLRT) {
    SR.T = 0;
}