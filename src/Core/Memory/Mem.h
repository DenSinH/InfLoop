#pragma once

#include "VideoInterface.h"
#include "InternalIO.h"

#include "default.h"
#include "MemoryHelpers.h"

#include <type_traits>
#include <string>
#include <intrin.h>

/* used for BIOS reads */
#define BIOS_NOP 0x0009
#define BIOS_RTS 0x000B

class Memory {

public:
    Memory(bool* paused);

    template<typename T, bool safe=false> T Read(u32 address);
    template<typename T> void Write(u32 address, T value);

    // debug stuff
    u16 ReadPalletteEntry(int index);

    void LoadROM(const std::string& file_name);

private:
    friend class Initializer;
    friend class LoopyPPU;

    bool* Paused;

    // a lot of these memory regions are not actually known what they do
    // most of this info is from the Mame driver (very lacking)
    //     @ https://github.com/mamedev/mame/blob/master/src/mame/drivers/casloopy.cpp
    // and from Lady Starbreeze (Marie) her WIP Casio loopy emu
    //     @ https://github.com/ladystarbreeze/lilySV100

    u8 RAM[0x8'0000]  = {};
    // based on 256x224 resolution at 16 bits per pixel
    // Animeland does a transfer of 7000h words to this region
    u8 Bitmap[0xe000]     = {};
    u8 TileMap[0x1000]    = {};
    u8 TileData[0xf000] = {};
    u8 PRAM[0x200]    = {};
    u8 ROM[0x20'0000] = {};  // 2MB ROMs
    u8 ORAM[0x400]    = {};  // on-chip RAM
    /* In the ROM headers of Animeland and Dream chase, the addresses 0x02000000 and 0x02001fff are set
     * My suspicion is this for the ROM header:
     * uint VBR
     * uint ROM_END
     * uint ID
     * uint FFFF'FFFF
     * uint SRAM_START
     * uint SRAM_END
     * filled with FFF until 0x0e000480
     * VBR table
     * */
    u8 SRAM[0x2000]   = {};

    VideoInterface IOVideoInterface = VideoInterface();

    static constexpr u8 BIOSStub[4] = {
        // RTS      NOP
        0x00, 0x0B, 0x00, 0x09
    };

    InternalIO IO = InternalIO();

    void DoDMA(int i, u16 DMACHCR);
    void CheckDMA();
};

template<typename T, bool safe>
T Memory::Read(u32 address) {
    // safe mode is for debugging purposes

    switch (address >> 24) {
        case 0x00:
            if constexpr(!safe) {
                log_warn("BIOS read @%08x", address);
            }
            return ReadArrayBE<T>(BIOSStub, address & 3);
        case 0x02:
            if (address < 0x0200'2000) {
                return ReadArrayBE<T>(SRAM, address & 0x1fff);
            }
            goto unhandled;
        case 0x05:
            if (address >= 0x05ff'ff00) {
                return IO.Read<T, safe>(address & 0xff);
            }
            goto unhandled;
        case 0x09:
            // RAM
            if (address < 0x0908'0000) {
                // let's not just assume mirroring
                return ReadArrayBE<T>(RAM, address & 0x7'ffff);
            }
            break;
        case 0x04:
        case 0x0c:
            // mirrors, judging from Marie's emulator
            switch ((address >> 12) & 0xfff) {
                case 0x000 ... 0x00e:
                    return ReadArrayBE<T>(Bitmap, address & 0xffff);
                case 0x040:
                    return ReadArrayBE<T>(TileMap, address & 0x0fff);
                case 0x041 ... 0x04f:
                    return ReadArrayBE<T>(TileData, (address & 0xffff) - 0x1000);
                case 0x051:
                    if ((address & 0xfff) < 0x200) {
                        return ReadArrayBE<T>(PRAM, address & 0x1ff);
                    }
                    else {
                        goto unhandled;
                    }
                case 0x058:
                    if ((address & 0xfff) < VideoInterface::size) {
                        return IOVideoInterface.Read<T, safe>(address);
                    }
                    goto unhandled;
                case 0x050:
                    // Animeland FUN_0e0147b4 might be interesting for this region
                    // sets of 2 ushort registers with flags
                case 0x052:
                case 0x059:
                case 0x05a:
                case 0x05b:
                case 0x05d:
                    /*
                     * Reads:
                     *      Animeland:
                     *          0c05d010
                     *          0c05d010
                     *          0c05d012
                     *          0c05d012
                     * */
                case 0x05e:
                    log_mem("Unhandled read: %08x", address);
                    return 0;
                default:
                    goto unhandled;
            }
        case 0x0e:
            if (address < 0x0e1f'ffff) {
                // ROM
                return ReadArrayBE<T>(ROM, address & 0x1f'ffff);
            }
            break;
        case 0x0f:
            if (address < 0x0f00'0400) {
                return ReadArrayBE<T>(ORAM, address & 0x3ff);
            }
            break;
        default:
            unhandled:
            break;
    }

    if constexpr(!safe) {
        log_fatal("Unknown %dbit read from %x", sizeof(T) << 3, address);
    }
    else {
        return 0;
    }
}

template<typename T>
void Memory::Write(u32 address, T value) {
//    if ((address >= 0x090136d0) && (address <= 0x090136d0 + 4)) {
//        *Paused = true;
//    }
//    if ((address >= 0x090138c4) && (address <= 0x090138c4 + 8)) {
//        *Paused = true;
//    }

    switch (address >> 24) {
        case 0x02:
            if (address < 0x0200'2000) {
                return WriteArrayBE<T>(SRAM, address & 0x1fff, value);
            }
            goto unhandled;
        case 0x09:
            // RAM
            WriteArrayBE<T>(RAM, address & 0x7'ffff, value);
            return;
        case 0x05:
            if (address >= 0x05ff'ff00) {
                IO.Write<T>(address & 0xff, value);
                switch (address & ~1) {
                    case 0x05ff'ff4e:  // DMA0CHCR
                    case 0x05ff'ff5e:  // DMA1CHCR
                    case 0x05ff'ff6e:  // DMA2CHCR
                    case 0x05ff'ff7e:  // DMA3CHCR
                    case 0x05ff'ff48:  // DMA3CHCR
                        CheckDMA();
                        break;
                }
                return;
            }
            goto unhandled;
        case 0x04:
        case 0x0c:
            // mirrors, judging from Marie's emulator
            switch ((address >> 12) & 0xfff) {
                case 0x000 ... 0x00e:
                    WriteArray<T>(Bitmap, address & 0xffff, value);
                    return;
                case 0x040:
                    WriteArrayBE<T>(TileMap, address & 0x0fff, value);
                    return;
                case 0x041 ... 0x04f:
                    WriteArrayBE<T>(TileData, (address & 0xffff) - 0x1000, value);
                    return;
                case 0x051:
                    if ((address & 0xfff) < 0x200) {
                        WriteArrayBE<T>(PRAM, address & 0x1ff, value);
                        return;
                    }
                    else {
                        goto unhandled;
                    }
                case 0x058:
                    if ((address & 0xfff) < VideoInterface::size) {
                        IOVideoInterface.Write<T>(address, value);
                        return;
                    }
                    goto unhandled;
                case 0x050:
                case 0x052:
                case 0x059:
                    /*
                     * Writes:
                     * Animeland:
                     *      4 to 0c059030
                     * */
                case 0x05a:
                    /*
                     * Writes:
                     * Animeland:
                     *      15   to 0c05a000
                     *      0    to 0c05a002
                     *      0    to 0c05a004
                     *      8    to 0c05a006
                     *      1c0  to 0c05a008
                     *      a987 to 0c05a00a
                     *      fedc to 0c05a00c
                     * */
                case 0x05b:
                    /*
                     * Writes:
                     * Animeland:
                     *      4 to c05b000
                     * */
                case 0x05e:
                    log_mem("Unhandled write: %x to %08x", value, address);
                    return;
                case 0x05f:
                    // This holds 0x80 ushorts of values
                    // Could this be some alternative PRAM (sticker printer?)
                    return;
                default:
                    goto unhandled;
            }
        case 0x0f:
            if (address < 0x0f00'0400) {
                WriteArrayBE<T>(ORAM, address & 0x3ff, value);
                return;
            }
            goto unhandled;
        default:
            unhandled:
            log_fatal("Unknown %dbit write %x to %x", sizeof(T) << 3, value, address);
    }
}