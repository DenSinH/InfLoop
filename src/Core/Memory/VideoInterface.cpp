#include "VideoInterface.h"

READ_PRECALL(VideoInterface::ReadVTR)  {
    // stub
    VTR = (VTR + 1) & 0x1ff;
    return VTR;
}

READ_PRECALL(VideoInterface::ReadOtherTimerReg)  {
    // stub
    OtherTimerReg = (OtherTimerReg + 1) & 0x1ff;
    return OtherTimerReg;
}