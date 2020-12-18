#include "Tables.h"


SH7021Instruction GetInstruction(u16 instruction) {
    switch (instruction & 0xff00) {
        case 0xc700:
            return &SH7021::MOVA;
        default:
            break;
    }

    switch (instruction & 0xf00f) {
        case 0x6003:
            return &SH7021::MOV<u32, AddressingMode::DirectRegister, AddressingMode::DirectRegister>;
        case 0x2000:
            return &SH7021::MOV<u8, AddressingMode::DirectRegister, AddressingMode::IndirectRegister>;
        case 0x2001:
            return &SH7021::MOV<u16, AddressingMode::DirectRegister, AddressingMode::IndirectRegister>;
        case 0x2002:
            return &SH7021::MOV<u32, AddressingMode::DirectRegister, AddressingMode::IndirectRegister>;
        case 0x6000:
            return &SH7021::MOV<u8, AddressingMode::IndirectRegister, AddressingMode::DirectRegister>;
        case 0x6001:
            return &SH7021::MOV<u16, AddressingMode::IndirectRegister, AddressingMode::DirectRegister>;
        case 0x6002:
            return &SH7021::MOV<u32, AddressingMode::IndirectRegister, AddressingMode::DirectRegister>;
        case 0x2004:
            return &SH7021::MOV<u8, AddressingMode::DirectRegister, AddressingMode::PreDecrementIndirectRegister>;
        case 0x2005:
            return &SH7021::MOV<u16, AddressingMode::DirectRegister, AddressingMode::PreDecrementIndirectRegister>;
        case 0x2006:
            return &SH7021::MOV<u32, AddressingMode::DirectRegister, AddressingMode::PreDecrementIndirectRegister>;
        case 0x6004:
            return &SH7021::MOV<u8, AddressingMode::PostIncrementIndirectRegister, AddressingMode::DirectRegister>;
        case 0x6005:
            return &SH7021::MOV<u16, AddressingMode::PostIncrementIndirectRegister, AddressingMode::DirectRegister>;
        case 0x6006:
            return &SH7021::MOV<u32, AddressingMode::PostIncrementIndirectRegister, AddressingMode::DirectRegister>;
            // todo: other addressing modes
    }

    return &SH7021::unimplemented;
}