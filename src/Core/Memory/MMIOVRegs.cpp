#include "MMIOVRegs.h"

READ_PRECALL(MMIOVRegs::ReadDISPSTAT)  {
    // VBlank status flag (polled by Animeland at
    DISPSTAT ^= 0x0100;
    return DISPSTAT;
}

WRITE_CALLBACK(MMIOVRegs::WriteDISPSTAT) {
    // save VBlank status flag
    DISPSTAT = (DISPSTAT & 0x0100) | (value & ~0x0100);
}