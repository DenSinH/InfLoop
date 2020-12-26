#include "SH7021.h"

/* Memcpy / memset routines:
 *
 * r4: ptr to stream of structs:
 * This struct seems to be padded to align by 4 bytes as follows:
 * struct {
 *  u16 flags;
 *  u16 _padding;
 *  u32 src_addr;
 *  u32 dest_addr;
 *  u16 len;  // units, not bytes
 *  u16 _padding;
 * }
 *
 * IF flags == 0xffff: END OF STREAM!
 *
 * This would then correspond with the DMA registers basically, except for the "flags" field
 * r5: DMA channel to use (for 66d0)
 *
 * It seems like no bits other than destination/source control would be used
 * other bits dont seem useful for a simple memcpy anyway.
 *
 *  - 1 is used to transfer PRAM to some pointer that is then cleared again
 *  - 1 is used to transfer from some IO register at 0x0c05'f000 into an array in RAM
 * and the main reason:
 *  - 4 is used to transfer a bitmap that was decompressed into RAM over to bitmap RAM at 0x0c00'0000
 *    if bitmap RAM is accessed directly, why wouldn't it just be decompressed directly in there?
 *
 *  - 5 is used to transfer 2 words from VRAM to cfff3000 (mirror of IO?)
 * */

void SH7021::BIOSMemcpyOrSet() {
    /*
     * (Originally this was just the call to 6a0e
     * The data passed to this function looks very very similar to that of memcpy.
     * Look at 0e004008 in Animeland to see a more obvious example of it's function.
     * The structs are the same size, I think the flag might indicate memcpy: see 0e003e5e/0e003e92 in Animeland
     * 6a0e is used as an "alternative" to 66d0, but actually there's just a different sort of struct of the same size on
     * the stack. This struct is pointed to by r4 and holds:
     * struct {
     *  u16 flags;
     *  u16 _padding;
     *  u32 dest;
     *  u16 mask;  // so for example f800: mem = (mem & mask) | value
     *  u16 value;
     *  u16 len;   // units, not bytes
     *  u16 _padding;
     * }
     *
     * IF flags == 0xffff: END OF STREAM!
     * this behavior of the flags actually explains the padding. There is not padding, rather there is just a ushort
     * in between every struct, but because these are smaller than words, and the struct would be 12 bytes long, there
     * is padding there!
     * */
    u32 data_ptr = R[4];  // first arg

    // flags seem to indicate whether it should be copied or set
    while (true) {
        u32 flags = Mem->Read<u16>(data_ptr);
        if (flags == 5) {
            u32 dest     = Mem->Read<u32>(data_ptr + 4);
            u16 mask     = Mem->Read<u16>(data_ptr + 8);
            u16 val      = Mem->Read<u16>(data_ptr + 10);
            u32 len      = Mem->Read<u16>(data_ptr + 12);
            log_debug("BIOS memset: %x, (%x & %04x) | %x, %x", flags, dest, mask, val, len);

            for (u32 i = 0; i < len; i++) {
                Mem->Write<u16>(dest, (Mem->Read<u16>(dest) & mask) | val);
                dest += 2;
            }
        }
        else if (flags == 0xffff) {
            break;
        }
        else if (flags == 0 || flags == 1 || flags == 4) {
            // these flags probably signify slight differences, but I haven't been able to deduce this
            u32 src      = Mem->Read<u32>(data_ptr + 4);
            u32 dest     = Mem->Read<u32>(data_ptr + 8);
            u32 len      = Mem->Read<u16>(data_ptr + 12);
            log_debug("BIOS memcpy: %x, %x -> %x, %x", flags, src, dest, len);
            for (u32 i = 0; i < len; i++) {
                Mem->Write<u16>(dest, Mem->Read<u16>(src));
                dest += 2;
                src  += 2;
            }
        }
        else {
            log_fatal("Unknown BIOS memcpy flag setting: %x", flags);
        }

        data_ptr += 0x10; // sizeof(struct)
    }
}

void SH7021::UnpackTile2BPP(u32 src, u32 dest, u32 offset) {
    for (int p = 0; p < 8 * 8; p += 4) {
        // 4 pixels per loop
        u8 PixelGroup = Mem->Read<u8>(src);  // 4 pixels at 2BPP
        src++;
        u16 result = 0;
        for (int sp = 0; sp < 4; sp++) {
            // subpixel
            result >>= 4;
            result |= ((PixelGroup & 3) + offset) << 12;
            PixelGroup >>= 2;
        }
        Mem->Write<u16>(dest, result);
        dest += 2;
    }
}

void SH7021::BIOSDouble2BPP8x81DTo4BPP16x162D6028() {
    /*
     * I think this function is similar to BIOS1BPP16x16To4BPP2D5f4c, except now it's for a single tile
     *
     * r4: pointer to tile data
     * r5: destination pointer
     * r6: count
     * r7: some pointer? maybe some usable buffer for the function to use while unpacking
     *
     * See Animeland function at 0e008002
     * Source is an array of 2BPP tile data
     * dest is some pointer incremented by 0x20 every loop
     * third argument is 1 here
     * fourth argument might be some pointer but I don't know
     *
     * Also look at 0e0099b0:
     * called with r6 = 0xda
     * So r6 cannot be an offset, it has to be some sort of length
     * a length of 0xda also corresponds with a full set of character tiles it seems
     *
     * After PPU research, I have realized that the Kana Animeland is trying to unpack are supposed to be mapped as 2D
     * tiles, and enlarged as well. So basically, this goes from 2 2BPP 8x8 tiles (top and bottom)
     * to a 4BPP 16x16 2D mapped tile.
     *
     * It unpacks 0xda tiles to 90791ac, and later copies from as far as 907feac
     * Now, notice that
     * 0x91ac + 0xda * 0x40 * 2 = 0xfeac
     * (start address + number of tiles * size of double width tile * 2 double width tiles)
     * this can't be a coincidence.
     * */
    u32 src    = R[4];
    u32 dest   = R[5];
    u32 count  = R[6];
    log_debug("BIOS tile unpack: %d x %x -> %x", count, src, dest);

    for (int i = 0; i < count; i++) {
        UnpackTile2BPP(src + 0x10 * i, dest + 0x20 * i, 0);
        // 4 16x16 tiles next to each other in 2D mapping
//        for (int j = 0; j < 4 && i < count; j++, i++) {
//            // upper tile
//            for (int p = 0; p < 8 * 8; p += 4) {
//                // 4 pixels per loop
//                u8 PixelGroup = Mem->Read<u8>(src);  // 4 pixels at 2BPP
//                src++;
//                u32 result = 0;  // making the tile wider
//                for (int sp = 0; sp < 4; sp++) { // subpixel
//                    result >>= 4;
//                    result |= (PixelGroup & 3) << 28;
//                    result >>= 4;
//                    result |= (PixelGroup & 3) << 28;
//                    PixelGroup >>= 2;
//                }
//                Mem->Write<u32>(dest, result);
//                dest += 4;
//            }
//
//            // lower
//            for (int p = 0; p < 8 * 8; p += 4) {
//                // 4 pixels per loop
//                u8 PixelGroup = Mem->Read<u8>(src);  // 4 pixels at 2BPP
//                src++;
//                u32 result = 0;  // making the tile wider
//                for (int sp = 0; sp < 4; sp++) { // subpixel
//                    result >>= 4;
//                    result |= (PixelGroup & 3) << 28;
//                    result >>= 4;
//                    result |= (PixelGroup & 3) << 28;
//                    PixelGroup >>= 2;
//                }
//                // extra offset for
//                Mem->Write<u32>(dest + 0x20 * 8, result);
//                dest += 4;
//            }
//        }
//
//        // account for 2D mapping
//        dest += 0x20 * 8;
    }
}

void SH7021::BIOS4x4TileUnpack2BPP60a4() {
    /*
     * I think this function is similar to BIOS1BPP16x16To4BPP2D5f4c, except now it's for a 4x4 set of 2BPP tiles
     *
     * r4: pointer to tile data
     * r5: destination pointer
     * r6: offset
     * r7: some pointer? maybe some usable buffer for the function to use while unpacking
     *
     * See Animeland function at 0e003a94
     * Source is a TileData struct -> data field:
     * {
     *      u32 flag;
     *      u8 data[256]
     * }
     * dest is some pointer incremented by 0x200 every loop
     * third argument is a default offset (?)
     * fourth argument might be some pointer but I doubt it
     * */
    u32 src    = R[4];
    u32 dest   = R[5];
    u32 offset = R[6];
    log_debug("BIOS 4x4 tile unpack: %x -> %x, +%x", src, dest, offset);

    for (u32 i = 0; i < 16; i++) {
        UnpackTile2BPP(src, dest, offset);
        src  += 0x10;
        dest += 0x20;
    }
}

void SH7021::BIOS1BPP16x16To4BPP2D5f4c() {
    /* Unpacks 16x16 1BPP tile data to 4 4BPP laid out in the 2D manner the PPU wants.
     *
     * r4: pointer to tile data
     * r5: destination pointer
     * r6: offset
     * r7: pointer to some struct set up by 6644?
     *     Does not seem to be used later, I don't know what this is for
     *
     * Animeland wants to use these kana with a 2D PPU mapping. It seems like the adjecent tiles should be widened
     * and placed below each other (at a 0x100 byte offset).
     * */
    u32 src    = R[4];
    u32 dest   = R[5];
    u32 offset = R[6];
    log_debug("BIOS kana unpack: %x -> %x, +%x", src, dest, offset);

    for (u32 offs : { 0, 0x100 }) {
        for (int dy = 0; dy < 8; dy++) {
            // 4 pixels per loop
            for (u32 x_offs : { 0, 0x20 }) {
                u8 PixelGroup = Mem->Read<u8>(src);  // 4 pixels at 2BPP
                src++;
                u32 result = 0;
                for (int sp = 0; sp < 8; sp++) {
                    // duplicate pixel
                    // actual loopy does something to make the lines more crisp
                    // I haven't figured out what yet
                    result >>= 4;
                    result |= ((PixelGroup & 1) + offset) << 28;
                    PixelGroup >>= 1;
                }
                Mem->Write<u32>(dest + x_offs + offs, result);
            }
            dest += 4;
        }
        // correct for tile offset (bottom tile should be at same offset within 2D mapping)
        dest -= 0x20;
//        UnpackTile2BPP(src, dest, offset);
//        src  += 0x10;
//        dest += 0x20;
    }
}

void SH7021::BIOSBitmapUncomp445c() {
    u32 data_struct_ptr = R[4];
    /* r4: ptr to uncomp data
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

void SH7021::BIOSMemcpy16_2f74() {
    /*
     * Look at 0e014284 in Animeland. This function is called, with 2 pointers and an int (8). The data
     * holds some stream, where the first int is 8. Then the first pointer passed to this function is used
     * as argument to copy over the data that was set up by this call. The first int is read as length again.
     * I think this is another memcpy routine for this, but a simple one:
     * memcpy16(src, dest, len (- 1?))
     * */
    u32 src  = R[4];
    u32 dest = R[5];
    u32 len  = R[6];
    log_debug("BIOS memcpy16: %08x -> %08x x %x", src, dest, len);

    for (int i = 0; i < len; i++) {
        Mem->Write<u16>(dest, Mem->Read<u16>(src));
        dest += 2;
        src  += 2;
    }
}

void SH7021::BIOSCall() {
    log_debug("BIOS call: %08x [PR -> PC](%08x -> %08x)", PC, PR, PC);
    log_debug("Arguments: r4-r7: [%08x, %08x, %08x, %08x]", R[4], R[5], R[6], R[7]);
    log_debug("Stack: [%08x, %08x, ...]", Mem->Read<u32, true>(R[15]), Mem->Read<u32, true>(R[15] + 4));
    /*
     * Known unknown calls:
     *  - 668:  directly on boot (boot screen of some sort?)
     *  - 6644: setup for KanaUnpack?
     *
     *  437c seems to be the same sort of compression as used by 445c, except 4 bits instead of 8 bits
     *  Arguments are
     *  r4: pointer to (packed) struct {
     *      // I guess the following 2 are just width * height
     *      u16 width;           // in nibbles
     *      u16 number_of_tiles; // in tiles
     *      byte* table;         // contains only nibbles
     *      byte* data_stream;
     *  }
     *  r5: destination pointer
     *  I don't think it has any other arguments
     * */

    switch(PC) {
        case 0x2f74:
            BIOSMemcpy16_2f74();
            break;
        case 0x437c:
            goto bios_call_default;
        case 0x445c:
            BIOSBitmapUncomp445c();
            break;
        case 0x5f4c:
            /* Animeland calls 6644 before this
             * it seems like 6644 is void (*)(int, int, void*)
             * Animeland calls it as (3, 2, u8[36]) // last value might be a struct of some sort
             * I suspect it might be setting up a DMA channel or something?
             * */
            BIOS1BPP16x16To4BPP2D5f4c();
            break;
        case 0x60a4:
            /*
             * See Animeland function at 0e003a94
             * BIOS 60d4 is also called. I suspect this BIOS call is to set values in OAM
             * void (*60d4)(byte/nibble flags_0 (8), byte/nibble flags_1 (9), byte/nibble flags_2 (10), byte index (i + 1), dest*)
             * dest* here is a pointer to a copy of ORAM, which is later copied back to ORAM
             * */
            BIOS4x4TileUnpack2BPP60a4();
            break;
        case 0x6028:
            /* Animeland function 0e008002:
             *  6028 called with:
             *      - pointer to 2BPP tile data
             *      - pointer to some location in VRAM
             *      - 1 number of tiles
             *      - pointer to 0f000000
             * I suspect this is another tile unpack function
             * */
            BIOSDouble2BPP8x81DTo4BPP16x162D6028();
            break;
        case 0x6a0e:
        case 0x66d0:
            // same as above, except with DMA channel (r5 argument)
            BIOSMemcpyOrSet();
            break;
        case 0x6644:
            /*
             * args: (int, int, void*)
             * area pointed to seems to be 32 bytes.
             * a pointer to this area is then also passed to the Kanaunpack routine
             * */
            goto bios_call_default;
        default:
        bios_call_default:
            log_debug("Unknown BIOS call");
            break;
    }

    // RET
    PC = PR;
}