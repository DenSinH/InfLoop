#include "MMIOBase.h"

enum class InternalIORegister : u32 {
    // DMA registers
    SAR0  = 0x40,
    DAR0  = 0x44,
    TCR0  = 0x4A,
    CHCR0 = 0x4E,
    SAR1  = 0x50,
    DAR1  = 0x54,
    TCR1  = 0x5A,
    CHCR1 = 0x5E,
    SAR2  = 0x60,
    DAR2  = 0x64,
    TCR2  = 0x6A,
    CHCR2 = 0x6E,
    SAR3  = 0x70,
    DAR3  = 0x74,
    TCR3  = 0x7A,
    CHCR3 = 0x7E,
    DMAOR = 0x48,

    // interrupt controller registers
    IPRA = 0x84,
    IPRB = 0x86,
    IPRC = 0x88,
    IPRD = 0x8A,
    IPRE = 0x8C,
};

class InternalIO : public MMIOBase<0x100, InternalIO> {
public:
    InternalIO() : MMIOBase<256, InternalIO>() {

    }

    constexpr void Init() override {
        SetReadWriteField<&InternalIO::DMAOR>(static_cast<u8>(InternalIORegister::DMAOR));
        SetImplemented(static_cast<u8>(InternalIORegister::SAR0));
        SetImplemented(static_cast<u8>(InternalIORegister::DAR0));
        SetImplemented(static_cast<u8>(InternalIORegister::TCR0));
        SetReadWriteField<&InternalIO::DMACHCR0>(static_cast<u8>(InternalIORegister::CHCR0));
        SetImplemented(static_cast<u8>(InternalIORegister::SAR1));
        SetImplemented(static_cast<u8>(InternalIORegister::DAR1));
        SetImplemented(static_cast<u8>(InternalIORegister::TCR1));
        SetReadWriteField<&InternalIO::DMACHCR1>(static_cast<u8>(InternalIORegister::CHCR1));
        SetImplemented(static_cast<u8>(InternalIORegister::SAR2));
        SetImplemented(static_cast<u8>(InternalIORegister::DAR2));
        SetImplemented(static_cast<u8>(InternalIORegister::TCR2));
        SetReadWriteField<&InternalIO::DMACHCR2>(static_cast<u8>(InternalIORegister::CHCR2));
        SetImplemented(static_cast<u8>(InternalIORegister::SAR3));
        SetImplemented(static_cast<u8>(InternalIORegister::DAR3));
        SetImplemented(static_cast<u8>(InternalIORegister::TCR3));
        SetReadWriteField<&InternalIO::DMACHCR3>(static_cast<u8>(InternalIORegister::CHCR3));
        SetImplemented(static_cast<u8>(InternalIORegister::IPRA));
        SetImplemented(static_cast<u8>(InternalIORegister::IPRB));
        SetImplemented(static_cast<u8>(InternalIORegister::IPRC));
        SetImplemented(static_cast<u8>(InternalIORegister::IPRD));
        SetImplemented(static_cast<u8>(InternalIORegister::IPRE));
    }

protected:

private:
    friend class Initializer;
    friend class Memory;

    u16 DMAOR;
    u16 DMACHCR0 = 0;
    u16 DMACHCR1 = 0;
    u16 DMACHCR2 = 0;
    u16 DMACHCR3 = 0;
};
