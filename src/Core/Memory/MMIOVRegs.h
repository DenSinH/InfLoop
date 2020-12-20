#pragma once

#include "MMIOBase.h"

// size of this is just a guess
class MMIOVRegs : public MMIOBase<0x100> {

public:
    MMIOVRegs() : MMIOBase<256>() {

    }

    constexpr void Init() override {
        SetImplemented(0x4);
        SetReadPrecall(0x4, &MMIOVRegs::ReadDISPSTAT);
        SetWriteCallback(0x4, &MMIOVRegs::WriteDISPSTAT);
    }

protected:

private:
    u16 DISPSTAT = 0;

    READ_PRECALL(ReadDISPSTAT);
    WRITE_CALLBACK(WriteDISPSTAT);
};