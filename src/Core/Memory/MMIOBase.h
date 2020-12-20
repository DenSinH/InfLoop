#pragma once

#include "MemoryHelpers.h"

#include "default.h"
#include "helpers.h"
#include "log.h"

#include <array>

template<size_t size> class MMIOBase;

#define READ_PRECALL(_name) u16 _name()
#define WRITE_CALLBACK(_name) void _name(u16 value)

class AbstractIO {};

using IOWriteCallback = void (AbstractIO::*)(u16 value);
using IOReadPrecall   = u16 (AbstractIO::*)();

template<size_t s>
class MMIOBase : AbstractIO {

    // s must be > 0 and a power of 2, otherwise our logic in reading/writing does not work
    static_assert(s && ((s & (s - 1)) == 0));

public:
    MMIOBase() {
        memset(Registers, 0, sizeof(Registers));
    }

    static constexpr size_t size = s;
    virtual constexpr void Init() {};

    template<typename T, bool safe=false> T Read(u32 address);
    template<typename T> void Write(u32 address, T value);

protected:

    u8 Registers[s] = {};

    void SetImplemented(u32 address) {
        Implemented[address >> 1] = true;
    }

    template<class Cb>
    void SetWriteCallback(u32 address, Cb cb) {
        WriteCallback[address >> 1] = static_cast<IOWriteCallback>(cb);
    }

    template<class Cb>
    void SetReadPrecall(u32 address, Cb cb) {
        ReadPrecall[address >> 1] = static_cast<IOReadPrecall>(cb);
    }

private:
    std::array<bool, (s >> 1)> Implemented = {false};
    std::array<IOWriteCallback, (s >> 1)> WriteCallback = {nullptr};
    std::array<IOReadPrecall, (s >> 1)> ReadPrecall     = {nullptr};

};

template<size_t s>
template<typename T, bool safe>
inline T MMIOBase<s>::Read(u32 address) {
    if constexpr (safe) {
        return ReadArrayBE<T>(Registers, address & (s - 1));
    }
    else {
        log_io("IO read from %08x", address);
    }

    if (!Implemented[(address & (s - 1)) >> 1]) {
        log_fatal("Unimplemented IO access at %08x", address);
    }

    address &= s - 1;
    if constexpr (std::is_same_v<T, u32>) {
        // 32 bit writes are a little different
        if (unlikely(ReadPrecall[address >> 1])) {
            WriteArray<u16>(Registers, address, ((this)->*ReadPrecall[address >> 1])());
        }
        if (unlikely(ReadPrecall[1 + (address >> 1)])) {
            WriteArray<u16>(Registers, address + 2, ((this)->*ReadPrecall[1 + (address >> 1)])());
        }

        return ReadArrayBE<T>(Registers, address);
    }
    else if constexpr (std::is_same_v<T, u16>) {
        if (unlikely(ReadPrecall[address >> 1])) {
            // no need for indirections for 16 bit reads
            return (this->*ReadPrecall[address >> 1])();
        }
        return ReadArrayBE<u16>(Registers, address);
    }
    else {
        if (unlikely(ReadPrecall[address >> 1])) {
            // we can also prevent indirections here
            // remember BIG Endianness!
            if (address & 1) {
                // misaligned
                return (u8)((this)->*ReadPrecall[address >> 1])();
            }
            return ((this)->*ReadPrecall[address >> 1])() >> 8;
        }
        return ReadArrayBE<u8>(Registers, address);
    }
}

template<size_t s>
template<typename T>
inline void MMIOBase<s>::Write(u32 address, T value) {
    if (!Implemented[(address & (s - 1)) >> 1]) {
        log_fatal("Unimplemented IO access at %08x", address);
    }
    log_io("IO write %x to %08x", value, address);

    address &= s - 1;
    if constexpr (std::is_same_v<T, u32>) {
        // 32 bit writes are a little different
        // Remember Big Endianness!
        WriteArrayBE<u16>(Registers, address, value);
        if (WriteCallback[address >> 1]) {
            (this->*WriteCallback[address >> 1])(value >> 16);
        }

        WriteArrayBE<u16>(Registers, address + 2, value >> 16);
        if (WriteCallback[1 + (address >> 1)]) {
            (this->*WriteCallback[1 + (address >> 1)])(value);
        }
    }
    else if constexpr (std::is_same_v<T, u16>) {
        WriteArrayBE<u16>(Registers, address, value);

        if (WriteCallback[address >> 1]) {
            (this->*WriteCallback[address >> 1])(value);
        }
    }
    else {
        WriteArrayBE<u8>(Registers, address, value);

        // todo: if there is also a special readcallback, the value read is not up to date
        if (WriteCallback[address >> 1]) {
            // We have to be careful with this:
            // writemask is already applied
            (this->*WriteCallback[address >> 1])(ReadArrayBE<u16>(Registers, address));
        }
    }
}