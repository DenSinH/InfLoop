#pragma once

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

    static constexpr u8 BIOSStub[4] = {
        // RTS      NOP
        0x00, 0x0B, 0x00, 0x09
    };
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
                        ReadArrayBE<T>(PRAM, address & 0x1ff);
                        break;
                    }
                    else {
                        goto unhandled;
                    }
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
            break;
        case 0x04:
        case 0x0c:
            // mirrors, judging from Marie's emulator
            switch ((address >> 12) & 0xfff) {
                case 0x040 ... 0x047:
                    WriteArrayBE<T>(VRAM, address & 0x7fff, value);
                    break;
                case 0x051:
                    if ((address & 0xfff) < 0x200) {
                        WriteArrayBE<T>(PRAM, address & 0x1ff, value);
                        break;
                    }
                    else {
                        goto unhandled;
                    }
                case 0x059:
                case 0x05a:
                    log_warn("Unhandled write: %x to %08x", value, address);
                    break;
                default:
                    goto unhandled;
            }
            break;
        default:
            unhandled:
            log_fatal("Unknown %dbit write %x to %x", sizeof(T) << 3, value, address);
            break;
    }
}