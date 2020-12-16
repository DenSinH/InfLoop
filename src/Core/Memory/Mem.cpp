#include "Mem.h"

Memory::Memory() {
    memset(PRAM, 0, sizeof(PRAM));
    memset(ROM, 0, sizeof(ROM));
}

// basically the same as Read<>, but I want this for debugging purposes
u8* Memory::GetPtr(u32 address) {
    switch (address >> 24) {
        case 0x0E:
            if (address < 0x0e1f'ffff) {
                // ROM
                return &ROM[address & 0x1f'ffff];
            }
        default:
            return nullptr;
    }
}