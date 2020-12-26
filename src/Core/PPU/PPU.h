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
    void* DrawScreen(u32 i);
    void OnTileDataClick(bool);
    void OnTileMapClick(bool);
    void OnScreenClick(u32 i, bool);

private:
    u32 TileDataPaletteBank = 0xc; // kind of random, but this is a good choice for animeland
    GLuint TileDataTexture;
    std::array<std::array<u16, 256>, 480> TileDataRaw = {};

    u32 TileMapHeatmapOffset = 0;
    GLuint TileMapTexture[2];
    std::array<std::array<u16, 32>, 28> TileMapRaw[2] = {};

    bool Screen2DMapping[2] = {};
    u32 ScreenPaletteBank[2] = {};
    GLuint ScreenTexture[2];
    std::array<std::array<u16, 256>, 224> ScreenRaw[2] = {};

    void DrawTile(u16* dest, size_t index, size_t target_width, u32 palette_bank, bool VFlip, bool HFlip, u32 palette_index);
    Memory* Mem;
};