#pragma once

#include "MemoryHelpers.h"

#include "default.h"
#include "helpers.h"
#include "log.h"

#include <array>

template<size_t size, class c> class MMIOBase;

#define READ_PRECALL(_name) u16 _name()
#define WRITE_CALLBACK(_name) void _name(u16 value)

template<class c>
using IOWriteCallback = void (c::*)(u16 value);
template<class c>
using IOReadPrecall   = u16 (c::*)();

template<size_t s, class c>
class MMIOBase {

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

    void SetImplemented(u32 address) {
        Implemented[address >> 1] = true;
    }

    void SetWriteCallback(u32 address, IOWriteCallback<c> cb) {
        WriteCallback[address >> 1] = cb;
    }

    void SetReadPrecall(u32 address, IOReadPrecall<c> cb) {
        ReadPrecall[address >> 1] = cb;
    }

private:

    std::array<bool, (s >> 1)> Implemented = {false};
    u8 Registers[s] = {};
    std::array<IOWriteCallback<c>, (s >> 1)> WriteCallback = {nullptr};
    std::array<IOReadPrecall<c>, (s >> 1)> ReadPrecall     = {nullptr};
};

template<size_t s, class c>
template<typename T, bool safe>
inline T MMIOBase<s, c>::Read(u32 address) {
    if constexpr (safe) {
        return ReadArrayBE<T>(Registers, address & (s - 1));
    }
    else {
        log_io("IO read from %08x", address);
    }

    if (!Implemented[(address & (s - 1)) >> 1]) {
        log_fatal("Unimplemented IO read at %08x", address);
    }

    address &= s - 1;
    if constexpr (std::is_same_v<T, u32>) {
        // 32 bit writes are a little different
        if (unlikely(ReadPrecall[address >> 1])) {
            WriteArray<u16>(Registers, address, (reinterpret_cast<c*>(this)->*ReadPrecall[address >> 1])());
        }
        if (unlikely(ReadPrecall[1 + (address >> 1)])) {
            WriteArray<u16>(Registers, address + 2, (reinterpret_cast<c*>(this)->*ReadPrecall[1 + (address >> 1)])());
        }

        return ReadArrayBE<T>(Registers, address);
    }
    else if constexpr (std::is_same_v<T, u16>) {
        if (unlikely(ReadPrecall[address >> 1])) {
            // no need for indirections for 16 bit reads
            return (reinterpret_cast<c*>(this)->*ReadPrecall[address >> 1])();
        }
        return ReadArrayBE<u16>(Registers, address);
    }
    else {
        if (unlikely(ReadPrecall[address >> 1])) {
            // we can also prevent indirections here
            // remember BIG Endianness!
            if (address & 1) {
                // misaligned
                return (u8)(reinterpret_cast<c*>(this)->*ReadPrecall[address >> 1])();
            }
            return (reinterpret_cast<c*>(this)->*ReadPrecall[address >> 1])() >> 8;
        }
        return ReadArrayBE<u8>(Registers, address);
    }
}

template<size_t s, class c>
template<typename T>
inline void MMIOBase<s, c>::Write(u32 address, T value) {
    if (!Implemented[(address & (s - 1)) >> 1]) {
        log_fatal("Unimplemented IO write %x to %08x", value, address);
    }
    log_io("IO write %x to %08x", value, address);

    address &= s - 1;
    if constexpr (std::is_same_v<T, u32>) {
        // 32 bit writes are a little different
        // Remember Big Endianness!
        WriteArrayBE<u16>(Registers, address, value);
        if (WriteCallback[address >> 1]) {
            (reinterpret_cast<c*>(this)->*WriteCallback[address >> 1])(value >> 16);
        }

        WriteArrayBE<u16>(Registers, address + 2, value >> 16);
        if (WriteCallback[1 + (address >> 1)]) {
            (reinterpret_cast<c*>(this)->*WriteCallback[1 + (address >> 1)])(value);
        }
    }
    else if constexpr (std::is_same_v<T, u16>) {
        WriteArrayBE<u16>(Registers, address, value);

        if (WriteCallback[address >> 1]) {
            (reinterpret_cast<c*>(this)->*WriteCallback[address >> 1])(value);
        }
    }
    else {
        WriteArrayBE<u8>(Registers, address, value);

        // todo: if there is also a special readcallback, the value read is not up to date
        if (WriteCallback[address >> 1]) {
            // We have to be careful with this:
            // writemask is already applied
            (reinterpret_cast<c*>(this)->*WriteCallback[address >> 1])(ReadArrayBE<u16>(Registers, address));
        }
    }
}