#pragma once

#include "MMIOBase.h"

// size of this is just a guess
class MMIOVRegs : public MMIOBase<0x100, MMIOVRegs> {

public:
    MMIOVRegs() : MMIOBase<256, MMIOVRegs>() {

    }

    constexpr void Init() override {
        Implemented[0x4 >> 1] = true;

        ReadPrecall[0x4 >> 1] = &MMIOVRegs::ReadDISPSTAT;
        WriteCallback[0x4 >> 1] = &MMIOVRegs::WriteDISPSTAT;
    }

protected:

private:
    u16 DISPSTAT = 0;

    READ_PRECALL(ReadDISPSTAT);
    WRITE_CALLBACK(WriteDISPSTAT);
};