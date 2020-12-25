#include "Tables.h"


SH7021Instruction GetInstruction(u16 instruction) {
    switch (instruction & 0xff00) {
        case 0x7000 ... 0x7f00:
            return &SH7021::ADDI;
        case 0x8000:
            return &SH7021::MOVregdisp<u8, false>;
        case 0x8100:
            return &SH7021::MOVregdisp<u16, false>;
        case 0x1000 ... 0x1f00:
            return &SH7021::MOVregdisp<u32, false>;
        case 0x8400:
            return &SH7021::MOVregdisp<u8, true>;
        case 0x8500:
            return &SH7021::MOVregdisp<u16, true>;
        case 0x5000 ... 0x5f00:
            return &SH7021::MOVregdisp<u32, true>;
        case 0x8800:
            return &SH7021::CMP<Condition::EQimm>;
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
        case 0xca00:
            return &SH7021::XORI;
        case 0xcb00:
            return &SH7021::ORI;
        case 0xcc00:
            return &SH7021::TSTB;
        case 0xcd00:
            return &SH7021::ANDB;
        case 0xce00:
            return &SH7021::XORB;
        case 0xcf00:
            return &SH7021::ORB;
        default:
            break;
    }

    switch (instruction & 0xf00f) {
        case 0x3000:
            return &SH7021::CMP<Condition::EQ>;
        case 0x3003:
            return &SH7021::CMP<Condition::GE>;
        case 0x3007:
            return &SH7021::CMP<Condition::GT>;
        case 0x3006:
            return &SH7021::CMP<Condition::HI>;
        case 0x3002:
            return &SH7021::CMP<Condition::HS>;
        case 0x200c:
            return &SH7021::CMP<Condition::STR>;
        case 0x2007:
            return &SH7021::DIV0S;
        case 0x3004:
            return &SH7021::DIV1;
        case 0x600b:
            return &SH7021::NEG;
        case 0x600a:
            return &SH7021::NEGC;
        case 0x3008:
            return &SH7021::SUB;
        case 0x300a:
            return &SH7021::SUBC;
        case 0x300c:
            return &SH7021::ADD;
        case 0x300e:
            return &SH7021::ADDC;
        case 0x300f:
            return &SH7021::ADDV;
        case 0x2009:
            return &SH7021::AND;
        case 0x200a:
            return &SH7021::XOR;
        case 0x200b:
            return &SH7021::OR;
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
        case 0x0004:
            return &SH7021::MOV<u8, AddressingMode::DirectRegister, AddressingMode::IndirectIndexedRegister>;
        case 0x0005:
            return &SH7021::MOV<u16, AddressingMode::DirectRegister, AddressingMode::IndirectIndexedRegister>;
        case 0x0006:
            return &SH7021::MOV<u32, AddressingMode::DirectRegister, AddressingMode::IndirectIndexedRegister>;
        case 0x000c:
            return &SH7021::MOV<u8, AddressingMode::IndirectIndexedRegister, AddressingMode::DirectRegister>;
        case 0x000d:
            return &SH7021::MOV<u16, AddressingMode::IndirectIndexedRegister, AddressingMode::DirectRegister>;
        case 0x000e:
            return &SH7021::MOV<u32, AddressingMode::IndirectIndexedRegister, AddressingMode::DirectRegister>;
        case 0x200e:
            return &SH7021::MULxW<false>;
        case 0x200f:
            return &SH7021::MULxW<true>;
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
        case 0x6008:
            return &SH7021::SWAPB;
        case 0x6009:
            return &SH7021::SWAPW;
        case 0x200d:
            return &SH7021::XTRCT;
        default:
            break;
    }

    switch (instruction & 0xf0ff) {
        case 0x0029:
            return &SH7021::MOVT;
        case 0x0002:
            return &SH7021::STC<ControlRegister::SR, AddressingMode::DirectRegister>;
        case 0x0012:
            return &SH7021::STC<ControlRegister::GBR, AddressingMode::DirectRegister>;
        case 0x0022:
            return &SH7021::STC<ControlRegister::VBR, AddressingMode::DirectRegister>;
        case 0x4003:
            return &SH7021::STC<ControlRegister::SR, AddressingMode::PreDecrementIndirectRegister>;
        case 0x4013:
            return &SH7021::STC<ControlRegister::GBR, AddressingMode::PreDecrementIndirectRegister>;
        case 0x4023:
            return &SH7021::STC<ControlRegister::VBR, AddressingMode::PreDecrementIndirectRegister>;
        case 0x4015:
            return &SH7021::CMP<Condition::PL>;
        case 0x4011:
            return &SH7021::CMP<Condition::PZ>;
        case 0x4000:
            return &SH7021::SHLL;
        case 0x4008:
            return &SH7021::SHLLn<2>;
        case 0x4018:
            return &SH7021::SHLLn<8>;
        case 0x4028:
            return &SH7021::SHLLn<16>;
        case 0x4001:
            return &SH7021::SHLR;
        case 0x4009:
            return &SH7021::SHLRn<2>;
        case 0x4019:
            return &SH7021::SHLRn<8>;
        case 0x4029:
            return &SH7021::SHLRn<16>;
        case 0x4021:
            return &SH7021::SHAR;
        case 0x4004:
            return &SH7021::ROTL;
        case 0x4005:
            return &SH7021::ROTR;
        case 0x4024:
            return &SH7021::ROTCL;
        case 0x4025:
            return &SH7021::ROTCR;
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
        case 0x000a:
            return &SH7021::STS<StatusRegister::MACH, AddressingMode::DirectRegister>;
        case 0x001a:
            return &SH7021::STS<StatusRegister::MACL, AddressingMode::DirectRegister>;
        case 0x002a:
            return &SH7021::STS<StatusRegister::PR, AddressingMode::DirectRegister>;
        case 0x4002:
            return &SH7021::STS<StatusRegister::MACH, AddressingMode::PreDecrementIndirectRegister>;
        case 0x4012:
            return &SH7021::STS<StatusRegister::MACL, AddressingMode::PreDecrementIndirectRegister>;
        case 0x4022:
            return &SH7021::STS<StatusRegister::PR, AddressingMode::PreDecrementIndirectRegister>;
        case 0x400a:
            return &SH7021::LDS<AddressingMode::DirectRegister, StatusRegister::MACH>;
        case 0x401a:
            return &SH7021::LDS<AddressingMode::DirectRegister, StatusRegister::MACL>;
        case 0x402a:
            return &SH7021::LDS<AddressingMode::DirectRegister, StatusRegister::PR>;
        case 0x4006:
            return &SH7021::LDS<AddressingMode::PostIncrementIndirectRegister, StatusRegister::MACH>;
        case 0x4016:
            return &SH7021::LDS<AddressingMode::PostIncrementIndirectRegister, StatusRegister::MACL>;
        case 0x4026:
            return &SH7021::LDS<AddressingMode::PostIncrementIndirectRegister, StatusRegister::PR>;
        case 0x402b:
            return &SH7021::JMP;
        default:
            break;
    }

    switch (instruction) {
        case 0x0008:
            return &SH7021::CLRT;
        case 0x0009:
            return &SH7021::NOP;
        case 0x000b:
            return &SH7021::RTS;
        case 0x0019:
            return &SH7021::DIV0U;
        default:
            break;
    }

    return &SH7021::unimplemented;
}