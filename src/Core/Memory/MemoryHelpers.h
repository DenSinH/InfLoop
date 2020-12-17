#pragma once

#include "default.h"
#include "helpers.h"
#include "log.h"

#include <type_traits>

template<typename T>
static ALWAYS_INLINE T ReadArray(const u8 array[], u32 address);

template<>
u8 ReadArray(const u8 array[], u32 address) {
    return array[address];
}

template<>
u16 ReadArray(const u8 array[], u32 address) {
#ifndef DIRTY_MEMORY_ACCESS
    address &= ~1;
    return (array[address + 1] << 8) | array[address];
#else
    return *(u16*)&array[address & ~1];
#endif
}

template<>
u32 ReadArray(const u8 array[], u32 address) {
#ifndef DIRTY_MEMORY_ACCESS
    address &= ~3;
    return *(u32*)&array[address];
#else
    return *(u32*)&array[address & ~3];
#endif
}

template<typename T>
static ALWAYS_INLINE T ReadArrayBE(const u8 array[], u32 address);

template<>
u8 ReadArrayBE(const u8 array[], u32 address) {
    return ReadArray<u8>(array, address);
}

template<>
u16 ReadArrayBE(const u8 array[], u32 address) {
    return bswap_16(ReadArray<u16>(array, address));
}

template<>
u32 ReadArrayBE(const u8 array[], u32 address) {
    return bswap_32(ReadArray<u32>(array, address));
}

template<typename T>
static ALWAYS_INLINE void WriteArray(u8 array[], u32 address, T value);

template<>
void WriteArray(u8 array[], u32 address, u8 value) {
    array[address] = value;
}

template<>
void WriteArray(u8 array[], u32 address, u16 value) {
#ifndef DIRTY_MEMORY_ACCESS
    address &= ~1;
    array[address]     = (value & 0xff);
    array[address + 1] = value >> 8;
#else
    *(u16*)&array[address & ~1] = value;
#endif
}

template<>
void WriteArray(u8 array[], u32 address, u32 value) {
#ifndef DIRTY_MEMORY_ACCESS
    address &= ~3;
    array[address]     =  value        & 0xff;
    array[address + 1] = (value >> 8)  & 0xff;
    array[address + 2] = (value >> 16) & 0xff;
    array[address + 3] = (value >> 24) & 0xff;
#else
    *(u32*)&array[address & ~3] = value;
#endif
}

template<typename T>
static ALWAYS_INLINE void WriteArrayBE(u8 array[], u32 address, T value);

template<>
void WriteArrayBE(u8 array[], u32 address, u8 value) {
    WriteArray<u8>(array, address, value);
}

template<>
void WriteArrayBE(u8 array[], u32 address, u16 value) {
    WriteArray<u16>(array, address, bswap_16(value));
}

template<>
void WriteArrayBE(u8 array[], u32 address, u32 value) {
    WriteArray<u32>(array, address, bswap_32(value));
}