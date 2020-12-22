#pragma once

#include "default.h"

#include "glad/glad.h"

#include <array>

class Memory;

class LoopyPPU {

public:
    LoopyPPU(Memory* mem) {
        this->Mem = mem;
    }

    void VideoInit();
    void* DrawTileData();
    void* DrawTileMap(u32 i);
    void OnTileDataClick(bool);
    void OnTileMapClick(bool);

private:
    static const u32 TileDataStart = 0x1000;

    u32 TileDataPaletteBank = 0xc; // kind of random, but this is a good choice for animeland
    GLuint TileDataTexture;
    std::array<std::array<u16, 256>, 256> TileDataRaw = {};

    u32 TileMapHeatmapOffset = 0;
    GLuint TileMapTexture[2];
    std::array<std::array<u16, 32>, 28> TileMapRaw[2] = {};

    void DrawTile(u16* dest, size_t index, size_t target_width, u32 palette_bank);
    Memory* Mem;
};