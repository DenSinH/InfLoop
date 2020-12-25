#include "Mem.h"

#include <fstream>

Memory::Memory(bool* paused) {
    IOVideoInterface.Init();
    IO.Init();

    memset(PRAM, 0, sizeof(PRAM));
    memset(ROM, 0, sizeof(ROM));

    this->Paused = paused;
}

size_t LoadFileTo(char* buffer, const std::string& file_name, size_t max_length) {
    std::ifstream infile(file_name, std::ios::binary);

    if (infile.fail()) {
        log_fatal("Failed to open file %s", file_name.c_str());
    }

    infile.seekg(0, std::ios::end);
    size_t length = infile.tellg();
    infile.seekg(0, std::ios::beg);

    if (length > max_length) {
        log_fatal("Failed loading file %s, buffer overflow: %x > %x", file_name.c_str(), length, max_length);
    }

    infile.read(buffer, length);
    log_info("Loaded %x bytes from file %s", length, file_name.c_str());
    return length;
}


void Memory::LoadROM(const std::string& file_path) {
    log_debug("Loading %s", file_path.c_str());
    LoadFileTo(reinterpret_cast<char *>(ROM), file_path, 0x0020'0000);
}

u16 Memory::ReadPalletteEntry(int index) {
    return ReadArrayBE<u16>(PRAM, index << 1);
}

void Memory::CheckDMA() {
    if (!(IO.DMAOR & 1)) {
        // DMAs disabled
        return;
    }

    // DE and not TE
    if ((IO.DMACHCR0 & 3) == 1) { DoDMA(0, IO.DMACHCR0); }
    if ((IO.DMACHCR1 & 3) == 1) { DoDMA(1, IO.DMACHCR1); }
    if ((IO.DMACHCR2 & 3) == 1) { DoDMA(2, IO.DMACHCR2); }
    if ((IO.DMACHCR3 & 3) == 1) { DoDMA(3, IO.DMACHCR3); }
}

constexpr i32 DeltaXAD(u16 mode) {
    switch(mode) {
        case 0b00:
            return 0;
        case 0b01:
            return 1;
        case 0b10:
            return -1;
        case 0b11:
        default:
            log_fatal("Illegal DMA address mode: %x", mode);
    }
}

void Memory::DoDMA(int i, u16 DMACHCR) {
    if ((DMACHCR & 0x0ff0) != 0x0c10) {
        log_fatal("Check DMA setting! Got %x", DMACHCR);
    }

    u32 size      = (DMACHCR & 8) ? 2 : 1;
    i32 delta_sad = DeltaXAD((DMACHCR >> 12) & 3) * size;
    i32 delta_dad = DeltaXAD((DMACHCR >> 14) & 3) * size;
    u32 sad = IO.Read<u32>(static_cast<u32>(InternalIORegister::SAR0) + 0x10 * i);
    u32 dad = IO.Read<u32>(static_cast<u32>(InternalIORegister::DAR0) + 0x10 * i);
    u32 len = IO.Read<u16>(static_cast<u32>(InternalIORegister::TCR0) + 0x10 * i);
    if (!len) {
        len = 0x10000;
    }
    log_dma("DMA %x -> %x, length %x size %d", sad, dad, len, size * 8);
    // *Paused = true;

    if (size == 1) {
        for (int _ = 0; _ < len; _++) {
            Write<u8>(dad, Read<u8>(sad));
            sad += delta_sad;
            dad += delta_dad;
        }
    }
    else {
        for (int _ = 0; _ < len; _++) {
            Write<u16>(dad, Read<u16>(sad));
            sad += delta_sad;
            dad += delta_dad;
        }
    }

    DMACHCR |= 2;  // transfer complete bit
    IO.Write<u16>(static_cast<u32>(InternalIORegister::CHCR0) + 0x10 * i, DMACHCR);
}