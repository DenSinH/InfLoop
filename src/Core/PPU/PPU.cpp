#include "PPU.h"

#include "DistinctColors.h"

#include "../Memory/MemoryHelpers.h"
#include "../Memory/Mem.h"

void LoopyPPU::VideoInit() {
    glGenTextures(1, &TileDataTexture);
    glBindTexture(GL_TEXTURE_2D, TileDataTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB5, TileDataRaw[0].size(), TileDataRaw.size(),
                 0, GL_RGB, GL_UNSIGNED_SHORT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glGenTextures(2, TileMapTexture);
    for (int i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, TileMapTexture[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB5, TileMapRaw[0][0].size(), TileMapRaw[0].size(),
                     0, GL_RGB, GL_UNSIGNED_SHORT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }

    log_debug("OpenGL error after video initialization: %x", glGetError());
}

void LoopyPPU::OnTileDataClick(bool left) {
    if (left) {
        TileDataPaletteBank++;
    }
    else {
        TileDataPaletteBank--;
    }
    TileDataPaletteBank &= 0xf;
}

void* LoopyPPU::DrawTileData() {
    size_t index = 0;
    for (size_t y = 0; y < 480; y += 8) {
        for (size_t x = 0; x < 256; x += 8) {
            DrawTile(&TileDataRaw[y][x], index, TileDataRaw[0].size(), TileDataPaletteBank);
            index++;
        }
    }

    glBindTexture(GL_TEXTURE_2D, TileDataTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0,
                    0, 0,
                    TileDataRaw[0].size(), TileDataRaw.size(),
                    GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, TileDataRaw.data()
    );
    return (void*)(intptr_t)TileDataTexture;
}

void LoopyPPU::DrawTile(u16 *dest, size_t index, size_t target_width, u32 palette_bank) {
    for (size_t y = 0; y < 8; y++) {
        for (size_t x = 0; x < 8; x++) {
            // 4bpp tiles
            u32 pixel_address = index * 0x20 + y * 4 + (x >> 1);
            pixel_address %= sizeof(Mem->TileData);
            u8 tile_data = Mem->TileData[pixel_address];

            if (x & 1) {
                tile_data &= 0xf;
            }
            else {
                tile_data >>= 4;
            }
            u16 color = ReadArrayBE<u16>(Mem->PRAM, (tile_data + (palette_bank << 4)) << 1);
            dest[y * target_width + x] = color;
        }
    }
}

void* LoopyPPU::DrawTileMap(u32 i) {
    for (int y = 0; y < 28; y++) {
        for (int x = 0; x < 32; x++) {
            u16 entry = ReadArrayBE<u16>(Mem->TileMap, i * 0x700 + 2 * ( y * 32 + x));
            TileMapRaw[i][y][x] = DistinctColors[(entry & 0xff) + TileMapHeatmapOffset];
        }
    }

    glBindTexture(GL_TEXTURE_2D, TileMapTexture[i]);
    glTexSubImage2D(GL_TEXTURE_2D, 0,
                    0, 0,
                    TileMapRaw[i][0].size(), TileMapRaw[i].size(),
                    GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, TileMapRaw[i].data()
    );
    return (void*)(intptr_t)TileMapTexture[i];
}

void LoopyPPU::OnTileMapClick(bool left) {
    if (left) {
        TileMapHeatmapOffset++;
    }
    else {
        TileMapHeatmapOffset--;
    }
    TileMapHeatmapOffset &= 0xff;
}

/*
 * How the tile map is built up:
 *  2 sections of 0x700 bytes (0x380 shorts). These shorts hold a tile ID (I think the bottom 8 or 9 bits)
 *  bits
 *  0 - 9: Tile ID
 *
 *
 * */