#include "Mem.h"

#include <fstream>

Memory::Memory() {
    VRegs.Init();

    memset(PRAM, 0, sizeof(PRAM));
    memset(ROM, 0, sizeof(ROM));
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