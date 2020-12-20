#include "SH7021.h"

void SH7021::BIOSMemcpy66d0() {
    u32 data_ptr = R[4];  // first arg
    u32 flags = Mem->Read<u16>(data_ptr);
    u32 dest  = Mem->Read<u32>(data_ptr + 4);
    u32 src   = Mem->Read<u32>(data_ptr + 8);
    u32 len   = Mem->Read<u16>(data_ptr + 12);
    log_debug("BIOS memcpy: %x, %x, %x, %x", flags, dest, src, len);

    if (flags == 1) {
        for (u32 i = 0; i < len; i++) {
            Mem->Write<u8>(dest, Mem->Read<u8>(src));
            dest++;
            src++;
        }
    }
    else {
        log_warn("Unknown BIOS memcpy flag setting: %x", flags);
    }
}

void SH7021::BIOSCall() {
    log_debug("BIOS call: %08x [PR -> PC](%08x -> %08x)", PC, PR, PC);
    log_debug("Arguments: r4-r7: [%08x, %08x, %08x, %08x]", R[4], R[5], R[6], R[7]);

    switch(PC) {
        case 0x66d0:
            // Memcpy like routine
            BIOSMemcpy66d0();
            break;
    }

    // RET
    PC = PR;
}