#ifndef INFLOOP_BIOSSTRUCTS_H
#define INFLOOP_BIOSSTRUCTS_H

struct s_BIOSMemcpy16Data {
    // BIOS call 66d0 / 6a0e (with certain flags)
    unsigned short flags;
    unsigned short padding_2;
    void* dest;
    void* src;
    unsigned short len;
    unsigned short padding_e;
};

struct s_BIOSMemsetData {
    // BIOS call 6a0e (with certain flags)
    unsigned short flags;
    unsigned short padding_2;
    void* dest;
    unsigned short mask;
    unsigned short val;
    unsigned short len;
    unsigned short padding_e;
};

#endif //INFLOOP_BIOSSTRUCTS_H
