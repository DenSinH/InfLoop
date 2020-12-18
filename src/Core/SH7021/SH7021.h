#pragma once

#include "Instruction.h"
#include "../Memory/Mem.h"

#include <array>
#include <type_traits>

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

typedef union {
    struct {
        u64 MACL: 32;
        i64 MACH: 10;
    };

    struct {
        i64 raw: 42;
    };
} s_MAC;

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

class SH7021 {
public:

    SH7021(Memory* Mem, i32* timer);

    void Step();

private:
    friend class Initializer;
    friend class SH7021INL;
    friend SH7021Instruction GetInstruction(u16 instruction);

    u32   R[16] = {};
    u32   PC    = 0x0e00'0480;  // ROM start
    u32   PR    = 0;   // Procedure Register
    s_MAC MAC   = {};  // Multiply and Accumulate registers
    s_SR  SR    = {};  // Status Register
    u32   GBR   = 0;   // Global Base Register (base of GBR addressing mode)
    u32   VBR   = 0;   // Vector Base Register (base of exception vector area)

    i32* timer;
    Memory* Mem;

    static std::array<SH7021Instruction, 0x10000> instructions;

    INSTRUCTION(unimplemented);
    INSTRUCTION(MOVA);

#define INLINED_INCLUDES
#include "Instructions/DataTransfer.inl"
#undef INLINED_INCLUDES

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

    template<typename T, AddressingMode src>
    u32 GetSrcOperand(const u8 m) {
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
            default:
                log_fatal("Unimplemented src addressing mode at PC = %08x", PC - 2);
                break;
        }
    }

    template<typename T, AddressingMode src, AddressingMode dest>
    void DoOperation(const u8 m, const u8 n, u32 (*operation)(u32 src_op, u32 dest_op)) {
        /*
         * OP.SIZE Rm, Rn
         * */
        u32 src_op = GetSrcOperand<T, src>(m);

        u32 dest_op;
        switch (dest) {
            case AddressingMode::DirectRegister:
                // OP.S ..., Rn
                R[n] = operation(src_op, R[n]);
                break;
            case AddressingMode::IndirectRegister:
                // OP.S ..., @Rn
                // todo: does this need sign extending?
                dest_op = SignExtend<T>(Mem->Read<T>(R[n]));
                Mem->Write<T>(R[n], operation(src_op, dest_op));
                break;
            case AddressingMode::PostIncrementIndirectRegister:
                // OP.S ..., @Rn+
                // todo: does this need sign extending?
                dest_op = SignExtend<T>(Mem->Read<T>(R[n]));
                R[n] += sizeof(T);
                Mem->Write<T>(R[n] - sizeof(T), operation(src_op, dest_op));
                break;
            case AddressingMode::PreDecrementIndirectRegister:
                // OP.S ..., @-Rn
                R[n] -= sizeof(T);
                // todo: does this need sign extending?
                dest_op = SignExtend<T>(Mem->Read<T>(R[n]));
                Mem->Write<T>(R[n], operation(src_op, dest_op));
                break;
            default:
                log_fatal("Unimplemented dest addressing mode at PC = %08x", PC - 2);
                break;
        }
    }
};