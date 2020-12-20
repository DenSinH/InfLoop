#pragma once

#include "MMIOBase.h"

// size of this is just a guess
class VideoInterface : public MMIOBase<0x100, VideoInterface> {

public:
    VideoInterface() : MMIOBase<256, VideoInterface>() {

    }

    constexpr void Init() override {
        SetImplemented(0x4);
        SetReadPrecall(0x4, &VideoInterface::ReadVTR);
    }

protected:

private:
    friend class Initializer;

    // it seems like 0c058004 is the register to communicate with the video system's current halfline
    // I think this because loopy is supposed to be plugged into a TV to play
    // this would mean that bit 1 being set, then cleared would mean its in VBlank I think
    u16 VTR = 0;

    READ_PRECALL(ReadVTR);
};