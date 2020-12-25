#pragma once

#include "Instruction.h"
#include "../Memory/Mem.h"

#include <array>
#include <type_traits>
#include <functional>

class SH7021;

typedef union {
    struct {
        u32 T: 1;
        u32 S: 1;
        u32:   2;
        u32 I: 4;
        u32 Q: 1;
        u32 M: 1;
        u32:  22;
    };

    u32 raw;
} s_SR;

#define INSTRUCTION(_name) void _name(s_instruction instruction)
#define CPU_INSTRUCTION(_name) INSTRUCTION(SH7021::_name)
typedef INSTRUCTION((SH7021::*SH7021Instruction));

enum class AddressingMode {
    DirectRegister,
    IndirectRegister,
    PostIncrementIndirectRegister,
    PreDecrementIndirectRegister,
    IndirectRegisterDisplacement,
    IndirectIndexedRegister,
    IndirectGBRDisplacement,
    IndirectIndexedGBR,
    PCRelativeDisplacement,
    PCRelativeD8,
    PCRelativeD12,
    PCRelativeRn,
    Immediate
};

enum class ControlRegister {
    SR,
    GBR,
    VBR,
};

enum class StatusRegister {
    MACH,
    MACL,
    PR
};

enum class Condition {
    EQ,
    GE,
    GT,
    HI,
    HS,
    PL,
    PZ,
    STR,
    EQimm
};

class SH7021 {
public:

    SH7021(Memory* Mem, i32* timer, bool* paused);

    void Step();
    void BIOSCall();

private:
    friend class Initializer;
    friend class SH7021INL;
    friend class Loopy;
    friend SH7021Instruction GetInstruction(u16 instruction);

    bool* Paused;

    // to prevent branches in branch delay slots
    bool InBranchDelay = false;

    u32   R[16] = {};
    u32   PC    = 0x0e00'0480; // ROM start
    u32   PR    = 0;           // Procedure Register
    u32   MACL  = 0;           // Multiply and Accumulate registers
    u32   MACH  = 0;           // Multiply and Accumulate registers
    s_SR  SR    = {.I = 0xf};  // Status Register
    u32   GBR   = 0;           // Global Base Register (base of GBR addressing mode)
    u32   VBR   = 0;           // Vector Base Register (base of exception vector area)

    i32* timer;
    Memory* Mem;

    static std::array<SH7021Instruction, 0x10000> instructions;

    INSTRUCTION(unimplemented);

    INSTRUCTION(MOVA);
    INSTRUCTION(MOVIimm);
    INSTRUCTION(MOVT);
    INSTRUCTION(SWAPB);
    INSTRUCTION(SWAPW);
    INSTRUCTION(XTRCT);

    INSTRUCTION(ADD);
    INSTRUCTION(ADDI);
    INSTRUCTION(ADDV);
    INSTRUCTION(ADDC);

    INSTRUCTION(SUB);
    INSTRUCTION(SUBC);

    INSTRUCTION(NEG);
    INSTRUCTION(NEGC);

    INSTRUCTION(DIV0U);
    INSTRUCTION(DIV0S);
    INSTRUCTION(DIV1);

    INSTRUCTION(TST);
    INSTRUCTION(TSTI);
    INSTRUCTION(TSTB);
    INSTRUCTION(AND);
    INSTRUCTION(ANDI);
    INSTRUCTION(ANDB);
    INSTRUCTION(OR);
    INSTRUCTION(ORI);
    INSTRUCTION(ORB);
    INSTRUCTION(XOR);
    INSTRUCTION(XORI);
    INSTRUCTION(XORB);

    INSTRUCTION(SHLL);
    INSTRUCTION(SHLR);
    INSTRUCTION(SHAR);
    INSTRUCTION(ROTL);
    INSTRUCTION(ROTR);
    INSTRUCTION(ROTCL);
    INSTRUCTION(ROTCR);

    ALWAYS_INLINE bool DelayBranch() {
        // note: PC will be 4 ahead on return "action" is executed
        // this is what it should be according to the spec

        if (unlikely(InBranchDelay)) {
            // invalid branch delay slot
            return false;
        }

        // signify we are in a branch delay to prevent certain instructions (mainly branches) from happening
        InBranchDelay = true;
        Step();
        InBranchDelay = false;

        // valid branch delay
        return true;
    }

    INSTRUCTION(BRA);
    INSTRUCTION(JSR);
    INSTRUCTION(JMP);
    INSTRUCTION(BSR);
    INSTRUCTION(RTS);

    INSTRUCTION(NOP) {};
    INSTRUCTION(CLRT);

#define INLINED_INCLUDES
#include "Instructions/DataTransfer.inl"
#include "Instructions/Control.inl"
#include "Instructions/Branch.inl"
#include "Instructions/Arithmetic.inl"
#include "Instructions/Shift.inl"
#undef INLINED_INCLUDES

    void BIOSBitmapUncomp445c();
    void UnpackTile2BPP(u32 src, u32 dest, u32 offset);
    void BIOSKanaUnpack5f4c();
    void BIOS4x4TileUnpack2BPP60a4();
    void BIOS2BPPTileUnpack6028();
    void BIOSMemcpyOrSet();

    template<typename T>
    static constexpr ALWAYS_INLINE u32 SignExtend(T value) {
        if constexpr(std::is_same_v<T, u8>) {
            return (u32)((i32)((i8)value));
        }
        else if constexpr(std::is_same_v<T, u16>) {
            return (u32)((i32)((i16)value));
        }
        else {
            return value;
        }
    }

    template<typename T, AddressingMode src, bool sext_imm = false>
    ALWAYS_INLINE u32 GetSrcOperand(const u8 m, const u16 instruction) {
        switch (src) {
            case AddressingMode::DirectRegister:
                // OP.S Rm, ...
                return R[m];
            case AddressingMode::IndirectRegister:
                // OP.S @Rm, ...
                return SignExtend<T>(Mem->Read<T>(R[m]));
            case AddressingMode::PostIncrementIndirectRegister:
                // OP.S @Rm+, ...
                R[m] += sizeof(T);
                return SignExtend<T>(Mem->Read<T>(R[m] - sizeof(T)));
            case AddressingMode::PreDecrementIndirectRegister:
                // OP.S @-Rm, ...
                R[m] -= sizeof(T);
                return SignExtend<T>(Mem->Read<T>(R[m]));
            case AddressingMode::PCRelativeD8:
                // OP.S @(d8, PC), ...
                if constexpr(std::is_same_v<T, u32>) {
                    // PC should be 4 bytes ahead, is 2 bytes ahead
                    return Mem->Read<T>(((PC + 2) & 0xffff'fffc) + ((instruction & 0xff) << 2));
                }
                else {
                    return SignExtend<T>(Mem->Read<T>((PC + 2) + ((instruction & 0xff) << 1)));
                }
            case AddressingMode::IndirectGBRDisplacement:
                // OP.S @(disp,GBR), ...
                return SignExtend<T>(Mem->Read<T>(GBR + sizeof(T) * (instruction & 0xff)));
            case AddressingMode::Immediate:
                if constexpr(sext_imm) {
                    return SignExtend<u8>(instruction & 0xff);
                }
                else {
                    return instruction & 0xff;
                }
            case AddressingMode::IndirectRegisterDisplacement:
                return SignExtend<T>(Mem->Read<T>(R[m] + (instruction & 0xf) * sizeof(T)));
            case AddressingMode::IndirectIndexedRegister:
                return SignExtend<T>(Mem->Read<T>(R[m] + R[0]));
            default:
                log_fatal("Unimplemented src addressing mode at PC = %08x", PC - 2);
        }
    }

    template<typename T, AddressingMode dest>
    ALWAYS_INLINE void DoWriteback(const u8 n, const u16 instruction, const T value) {
        switch (dest) {
            case AddressingMode::DirectRegister:
                // OP.S ..., Rn
                R[n] = value;
                break;
            case AddressingMode::IndirectRegister:
                // OP.S ..., @Rn
                Mem->Write<T>(R[n], value);
                break;
            case AddressingMode::PostIncrementIndirectRegister:
                // OP.S ..., @Rn+
                R[n] += sizeof(T);
                Mem->Write<T>(R[n] - sizeof(T), value);
                break;
            case AddressingMode::PreDecrementIndirectRegister:
                // OP.S ..., @-Rn
                R[n] -= sizeof(T);
                Mem->Write<T>(R[n], value);
                break;
            case AddressingMode::IndirectGBRDisplacement:
                // OP.S ..., @(disp,GBR)
                Mem->Write<T>(GBR + sizeof(T) * (instruction & 0xff), value);
                break;
            case AddressingMode::IndirectIndexedGBR:
                // OP.S ..., @(disp,GBR)
                Mem->Write<T>(GBR + R[0], value);
                break;
            case AddressingMode::IndirectRegisterDisplacement:
                Mem->Write<T>(R[n] + (instruction & 0xf) * sizeof(T), value);
                break;
            case AddressingMode::IndirectIndexedRegister:
                Mem->Write<T>(R[n] + R[0], value);
                break;
            default:
                log_fatal("Unimplemented dest storeback mode at PC = %08x", PC - 2);
                break;
        }
    }

    template<typename T, AddressingMode src, AddressingMode dest, bool BinOp>
    ALWAYS_INLINE void DoOperation(const u8 m, const u8 n, const u16 instruction, u32 (*operation)(u32 src_op, u32 dest_op)) {
        /*
         * OP.SIZE Rm, Rn
         * */
        u32 src_op = GetSrcOperand<T, src>(m, instruction);

        u32 dest_op = 0;
        switch (dest) {
            case AddressingMode::DirectRegister:
                // OP.S ..., Rn
                R[n] = operation(src_op, R[n]);
                break;
            case AddressingMode::IndirectRegister:
                // OP.S ..., @Rn
                if constexpr(BinOp) {
                    // todo: does this need sign extending?
                    dest_op = SignExtend<T>(Mem->Read<T>(R[n]));
                }
                Mem->Write<T>(R[n], operation(src_op, dest_op));
                break;
            case AddressingMode::PostIncrementIndirectRegister:
                // OP.S ..., @Rn+
                if constexpr(BinOp) {
                    // todo: does this need sign extending?
                    dest_op = SignExtend<T>(Mem->Read<T>(R[n]));
                }
                R[n] += sizeof(T);
                Mem->Write<T>(R[n] - sizeof(T), operation(src_op, dest_op));
                break;
            case AddressingMode::PreDecrementIndirectRegister:
                // OP.S ..., @-Rn
                R[n] -= sizeof(T);
                if constexpr(BinOp) {
                    // todo: does this need sign extending?
                    dest_op = SignExtend<T>(Mem->Read<T>(R[n]));
                }
                Mem->Write<T>(R[n], operation(src_op, dest_op));
                break;
            case AddressingMode::IndirectGBRDisplacement:
                // OP.S ..., @(disp,GBR)
                if constexpr(BinOp) {
                    // todo: does this need sign extending?
                    dest_op = SignExtend<T>(Mem->Read<T>(GBR + sizeof(T) * (instruction & 0xff)));
                }
                Mem->Write<T>(GBR + sizeof(T) * (instruction & 0xff), operation(src_op, dest_op));
                break;
            case AddressingMode::IndirectIndexedGBR:
                // OP.S ..., @(disp,GBR)
                if constexpr(BinOp) {
                    // todo: does this need sign extending?
                    dest_op = SignExtend<T>(Mem->Read<T>(GBR + R[0]));
                }
                Mem->Write<T>(GBR + R[0], operation(src_op, dest_op));
                break;
            case AddressingMode::IndirectRegisterDisplacement:
                if constexpr(BinOp) {
                    // todo: does this need sign extending?
                    dest_op = SignExtend<T>(Mem->Read<T>(R[n] + (instruction & 0xf) * sizeof(T)));
                }
                Mem->Write<T>(R[n] + (instruction & 0xf) * sizeof(T), operation(src_op, dest_op));
                break;
            case AddressingMode::IndirectIndexedRegister:
                if constexpr(BinOp) {
                    // todo: does this need sign extending?
                    dest_op = SignExtend<T>(Mem->Read<T>(R[n] + R[0]));
                }
                Mem->Write<T>(R[n] + R[0], operation(src_op, dest_op));
                break;
            default:
                log_fatal("Unimplemented dest addressing mode at PC = %08x", PC - 2);
                break;
        }
    }
};