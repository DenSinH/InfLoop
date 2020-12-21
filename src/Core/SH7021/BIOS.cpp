#include "SH7021.h"

void SH7021::BIOSMemcpy66d0() {
    /*
     *
     * */

    u32 data_ptr = R[4];  // first arg
    u32 flags = Mem->Read<u16>(data_ptr);
    u32 src  = Mem->Read<u32>(data_ptr + 4);
    u32 dest   = Mem->Read<u32>(data_ptr + 8);
    u32 len   = Mem->Read<u16>(data_ptr + 12);
    log_debug("BIOS memcpy: %x, %x -> %x, %x", flags, src, dest, len);

    if (flags == 1) {
        /*
         * Judging from FUN_0e00eda0 in Animeland (specifically:
         *  ushort local3c = 0xffff;
         *  for (int i = 0; i < 4; i++) {
         *    bios_6a48(&local3c);
         *    waitforvblank();
         *    int var4 = i + 1;
         *    int dest = 0x100 * i + ptr_into_huge_struct;
         *    int var5 = 0x300 + i;
         *    while (var5 < 0x3e0) {
         *      *c05b00a = var5;
         *      *c058006 = 1;
         *      while (*c058004 & 0xff != var4) {}
         *      data_struct.dest = dest;  // ??
         *      (bios_66d0)(data_struct, 3);
         *      var4 += 4;
         *      dest += 0x400;  // <-- THIS, length in struct holds 80, loop is 4 times
         *                      // I suspect that this means 100h bytes per loop, which would mean word transfers
         *      var5 += 4;
         *    }
         *  }
         * */
        for (u32 i = 0; i < len; i++) {
            Mem->Write<u16>(dest, Mem->Read<u16>(src));
            dest += 2;
            src += 2;
        }
    }
    else {
        log_warn("Unknown BIOS memcpy flag setting: %x", flags);
    }
}

void SH7021::BIOSBitmapUncomp445c() {
    u32 data_struct_ptr = R[4];
    /*
     * struct BitmapUncompData {
     *     u16 width;
     *     u16 height;
     *     void* header;
     *     void* data;
     * }
     * */
    u32 dest = R[5];
    u16 width = Mem->Read<u16>(data_struct_ptr);
    u16 height = Mem->Read<u16>(data_struct_ptr + 2);
    u32 header_ptr = Mem->Read<u16>(data_struct_ptr + 4);
    u32 data_ptr = Mem->Read<u16>(data_struct_ptr + 4);
    /*
     * The header seems to hold the following info:
     *  1 byte:  N, header table size
     *  1 byte:  some value (number of tables?) (2 for Animeland)
     *  N bytes: header table, filled with unique values
     * */
}

void SH7021::BIOSCall() {
    log_debug("BIOS call: %08x [PR -> PC](%08x -> %08x)", PC, PR, PC);
    log_debug("Arguments: r4-r7: [%08x, %08x, %08x, %08x]", R[4], R[5], R[6], R[7]);

    switch(PC) {
        case 0x445c:
            BIOSBitmapUncomp445c();
            break;
        case 0x66d0:
            // Memcpy like routine
            BIOSMemcpy66d0();
            break;
        default:
            log_debug("Unknown BIOS call");
            break;
    }

    // RET
    PC = PR;
}