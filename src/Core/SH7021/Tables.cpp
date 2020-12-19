#include "Tables.h"


SH7021Instruction GetInstruction(u16 instruction) {
    switch (instruction & 0xff00) {
        case 0x7000 ... 0x7f00:
            return &SH7021::ADDI;
        case 0x8900:
            return &SH7021::Bt<true>;
        case 0x8b00:
            return &SH7021::Bt<false>;
        case 0x9000 ... 0x9f00:
            return &SH7021::MOVI<u16>;
        case 0xa000 ... 0xaf00:
            return &SH7021::BRA;
        case 0xd000 ... 0xdf00:
            return &SH7021::MOVI<u32>;
        case 0xe000 ... 0xef00:
            return &SH7021::MOVIimm;
        case 0xb000 ... 0xbf00:
            return &SH7021::BSR;
        case 0xc400:
            return &SH7021::MOVgbrdisp<u8, true>;
        case 0xc500:
            return &SH7021::MOVgbrdisp<u16, true>;
        case 0xc600:
            return &SH7021::MOVgbrdisp<u32, true>;
        case 0xc000:
            return &SH7021::MOVgbrdisp<u8, false>;
        case 0xc100:
            return &SH7021::MOVgbrdisp<u16, false>;
        case 0xc200:
            return &SH7021::MOVgbrdisp<u32, false>;
        case 0xc700:
            return &SH7021::MOVA;
        case 0xc800:
            return &SH7021::TSTI;
        case 0xc900:
            return &SH7021::ANDI;
        case 0xcc00:
            return &SH7021::TSTB;
        case 0xcd00:
            return &SH7021::ANDB;
        default:
            break;
    }

    switch (instruction & 0xf00f) {
        case 0x300c:
            return &SH7021::ADD;
        case 0x2009:
            return &SH7021::AND;
        case 0x6003:
            return &SH7021::MOV<u32, AddressingMode::DirectRegister, AddressingMode::DirectRegister>;
        case 0x2000:
            return &SH7021::MOV<u8, AddressingMode::DirectRegister, AddressingMode::IndirectRegister>;
        case 0x2001:
            return &SH7021::MOV<u16, AddressingMode::DirectRegister, AddressingMode::IndirectRegister>;
        case 0x2002:
            return &SH7021::MOV<u32, AddressingMode::DirectRegister, AddressingMode::IndirectRegister>;
        case 0x2008:
            return &SH7021::TST;
        case 0x6000:
            return &SH7021::MOV<u8, AddressingMode::IndirectRegister, AddressingMode::DirectRegister>;
        case 0x6001:
            return &SH7021::MOV<u16, AddressingMode::IndirectRegister, AddressingMode::DirectRegister>;
        case 0x6002:
            return &SH7021::MOV<u32, AddressingMode::IndirectRegister, AddressingMode::DirectRegister>;
        case 0x600c:
            return &SH7021::EXTU<u8>;
        case 0x600d:
            return &SH7021::EXTU<u16>;
        case 0x600e:
            return &SH7021::EXTS<u8>;
        case 0x600f:
            return &SH7021::EXTS<u16>;
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

    switch (instruction & 0xf0ff) {
        case 0x4008:
            return &SH7021::SHLLn<2>;
        case 0x4018:
            return &SH7021::SHLLn<8>;
        case 0x4048:
            return &SH7021::SHLLn<16>;
        case 0x4009:
            return &SH7021::SHLRn<2>;
        case 0x4019:
            return &SH7021::SHLRn<8>;
        case 0x4049:
            return &SH7021::SHLRn<16>;
        case 0x400e:
            return &SH7021::LDC<AddressingMode::DirectRegister, ControlRegister::SR>;
        case 0x401e:
            return &SH7021::LDC<AddressingMode::DirectRegister, ControlRegister::GBR>;
        case 0x402e:
            return &SH7021::LDC<AddressingMode::DirectRegister, ControlRegister::VBR>;
        case 0x4007:
            return &SH7021::LDC<AddressingMode::PostIncrementIndirectRegister, ControlRegister::SR>;
        case 0x4017:
            return &SH7021::LDC<AddressingMode::PostIncrementIndirectRegister, ControlRegister::GBR>;
        case 0x4027:
            return &SH7021::LDC<AddressingMode::PostIncrementIndirectRegister, ControlRegister::VBR>;
        case 0x400b:
            return &SH7021::JSR;
        case 0x002a:
            return &SH7021::STS<StatusRegister::PR, AddressingMode::DirectRegister>;
        case 0x4022:
            return &SH7021::STS<StatusRegister::PR, AddressingMode::PreDecrementIndirectRegister>;
        case 0x402a:
            return &SH7021::LDS<AddressingMode::DirectRegister, StatusRegister::PR>;
        case 0x4026:
            return &SH7021::LDS<AddressingMode::PreDecrementIndirectRegister, StatusRegister::PR>;
    }

    switch (instruction) {
        case 0x0009:
            return &SH7021::NOP;
        case 0x000b:
            return &SH7021::RTS;
    }

    return &SH7021::unimplemented;
}