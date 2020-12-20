#pragma once

#include "MMIOVRegs.h"

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
    Memory();

    template<typename T, bool safe=false> T Read(u32 address);
    template<typename T> void Write(u32 address, T value);

    // debug stuff
    u8 ReadByteSafe(u32 address);
    u16 ReadWordSafe(u32 address);
    u16 ReadPalletteEntry(int index);

    void LoadROM(const std::string& file_name);

private:
    friend class Initializer;

    // a lot of these memory regions are not actually known what they do
    // most of this info is from the Mame driver (very lacking)
    //     @ https://github.com/mamedev/mame/blob/master/src/mame/drivers/casloopy.cpp
    // and from Lady Starbreeze (Marie) her WIP Casio loopy emu
    //     @ https://github.com/ladystarbreeze/lilySV100
    u8 RAM[0x8'0000]  = {};
    u8 VRAM[0x8'0000] = {};
    u8 PRAM[0x200]    = {};
    u8 ROM[0x20'0000] = {};  // 2MB ROMs

    MMIOVRegs VRegs = MMIOVRegs();

    static constexpr u8 BIOSStub[4] = {
        // RTS      NOP
        0x00, 0x0B, 0x00, 0x09
    };

    struct {
        u16 DMAOR;   // 0x05ff'ff48
        u16 IPR[5];  // A - E, 0x05ff'ff84, 86, 88, 8a, 8c
    } InternalIO;
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
                case 0x040 ... 0x047:
                    return ReadArrayBE<T>(VRAM, address & 0x7fff);
                case 0x051:
                    if ((address & 0xfff) < 0x200) {
                        return ReadArrayBE<T>(PRAM, address & 0x1ff);
                    }
                    else {
                        goto unhandled;
                    }
                case 0x058:
                    if ((address & 0xfff) < MMIOVRegs::size) {
                        return VRegs.Read<T, safe>(address);
                    }
                    goto unhandled;
                case 0x05d:
                    /*
                     * Reads:
                     *      Animeland:
                     *          0c05d010
                     *          0c05d010
                     *          0c05d012
                     *          0c05d012
                     * */
                    log_warn("Unhandled read: %08x", address);
                    return 0;
                default:
                    goto unhandled;
            }
        case 0x0E:
            if (address < 0x0e1f'ffff) {
                // ROM
                return ReadArrayBE<T>(ROM, address & 0x1f'ffff);
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
    switch (address >> 24) {
        case 0x09:
            // RAM
            WriteArrayBE<T>(RAM, address & 0x7'ffff, value);
            return;
        case 0x05:
            if constexpr(std::is_same_v<T, u16>) {
                switch (address) {
                    case 0x05ff'ff48:
                        InternalIO.DMAOR = value;
                        return;
                    case 0x05ff'ff84 ... 0x05ff'ff8c:
                        InternalIO.IPR[(address - 0x05ff'ff84) >> 1] = value;
                        return;
                    default:
                        break;
                }
            }
            goto unhandled;
        case 0x04:
        case 0x0c:
            // mirrors, judging from Marie's emulator
            switch ((address >> 12) & 0xfff) {
                case 0x040 ... 0x047:
                    WriteArrayBE<T>(VRAM, address & 0x7fff, value);
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
                    if ((address & 0xfff) < MMIOVRegs::size) {
                        VRegs.Write<T>(address, value);
                        return;
                    }
                    goto unhandled;
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
                    log_warn("Unhandled write: %x to %08x", value, address);
                    return;
                default:
                    goto unhandled;
            }
            break;
        default:
            unhandled:
            log_fatal("Unknown %dbit write %x to %x", sizeof(T) << 3, value, address);
    }
}