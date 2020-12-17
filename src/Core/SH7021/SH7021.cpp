#include "SH7021.h"

#include "Tables.h"

SH7021::SH7021(Memory* Mem, i32* timer) {
    this->Mem = Mem;
    this->timer = timer;
}

void SH7021::Step() {
    u16 opcode = Mem->Read<u16>(PC);
    PC += 2;

    SH7021::instructions[opcode](this, s_instruction{.raw = opcode});
    (*timer)++;
}
