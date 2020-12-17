#pragma once

#include "Interpreter.h"
#include "Tables.h"
#include "../Memory/Mem.h"

#include <array>

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

class SH7021 {
public:

    SH7021(Memory* Mem, i32* timer);

    void Step();

private:
    friend class Interpreter;
    friend class Initializer;

    u32   R[16] = {};
    u32   PC    = 0x0e00'0480;  // ROM start
    u32   PR    = 0;   // Procedure Register
    s_MAC MAC   = {};  // Multiply and Accumulate registers
    s_SR  SR    = {};  // Status Register
    u32   GBR   = 0;   // Global Base Register (base of GBR addressing mode)
    u32   VBR   = 0;   // Vector Base Register (base of exception vector area)

    i32* timer;
    Memory* Mem;

    static constexpr std::array<INSTRUCTION((*)), 0x10000> instructions = [] {
        std::array<INSTRUCTION((*)), 0x10000> table = {};

        for (size_t i = 0; i < 0x10000; i++) {
            table[i] = GetInstruction(i);
        }

        return table;
    }();;
};