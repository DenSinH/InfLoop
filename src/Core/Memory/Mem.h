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

    template<typename T> T Read(u32 address);
    template<typename T> void Write(u32 address, T value);

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
    u8 RAM[0x8'0000];
    u8 PRAM[0x200];
    u8 ROM[0x20'0000];  // 2MB ROMs
};

template<typename T>
T Memory::Read(u32 address) {
    switch (address >> 24) {
        case 0x0E:
            if (address < 0x0e1f'ffff) {
                // ROM
                // ROMs are usually dumped as big endian
                // I dont know why, but we will need to account for this
                if constexpr(std::is_same_v<T, u8>) {
                    return ReadArray<u8>(ROM, address & 0x1f'ffff);
                }
                else if constexpr(std::is_same_v<T, u16>) {
                    return _byteswap_ushort(ReadArray<u16>(ROM, address & 0x1f'ffff));
                }
                else {
                    return _byteswap_ulong(ReadArray<u32>(ROM, address & 0x1f'ffff));
                }
            }
        default:
            log_fatal("Unknown %dbit read from %x", sizeof(T) << 3, address);
            break;
    }
}

template<typename T>
void Memory::Write(u32 address, T value) {
    switch (address >> 24) {
        default:
            log_fatal("Unknown %dbit read from %x", sizeof(T) << 3, address);
            break;
    }
}