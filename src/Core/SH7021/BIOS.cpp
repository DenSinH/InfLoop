#include "SH7021.h"

void SH7021::BIOSCall() {
    log_debug("BIOS call: %08x [PR -> PC](%08x -> %08x)", PC - 2, PR, PC - 2);
    log_debug("Arguments: r4-r7: [%08x, %08x, %08x, %08x]", R[4], R[5], R[6], R[7]);

    // RET
    PC = PR;
}