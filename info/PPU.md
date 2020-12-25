## The PPU
So now I have tiles in VRAM for Animeland that I know should go where and what palette. The buttons on the bottom right seem
the most obvious to me, so lets look at those.
They are at 9 rows from the top of the tile map. Each row is 32 tiles (4 rows in my memory viewer).
That means they should be in the tile map around 0x700 (tile map 1) + 0x200 (rows above it) + 0x30 (partial rows) ~= 0x920 into VRAM.
Tile entries (that are not background) are:
```
... 29 80  29 82 ... 
... 29 84  29 86 ...
... 29 90  29 92 ...
... 29 94  29 96 ...
... 29 a0  29 a2 ...
```
Selected buttons look the same, except `39 ..`. This means that the palette is dictated by bits 11/12. There has to be some
register selecting a palette for the tile map as well. The buttons that are supposed to go on these spots have sprites of 4x2 tiles. There definitely seems to be 
some sort of scaling going on. The Tile map does not fill the entire 0x700 byte area dedicated to it either. 
The tiles are also laid out in memory as ttttTTTTbbbbBBBB for top/bottom halves of the tiles. In this "mode", 
the tile IDs probably mean 2x2 groups of tiles, and the tile map is a 2D mapping, with a width of 8 tiles.
Tile entries start at `0x4400` into VRAM, or `0x3400` into the tile data section. This corresponds with IDs of 
`1a0` and up. Now if the tile data actually holds `f000` bytes, then we can never fit this many IDs (`0x780` of them) in 8 or 9 bits.
So there must be an IO register showing what bank to use for the tile map, or this must be specified in the tile map entry.
There are 3 cases:
    - 7 bit (or less) tile IDs: In this case, the tile ID "offset" register must specify that these tiles must start at a `0x3000` byte offset into the tile data region.
    - 8 bit tile IDs: In this case all tile IDs are `0x80` and up, so the offset must be `0x1000`.
    - 9-11 bit tile IDs: In this case all tile IDs are `0x180` and up, so the offset must be `0`, or no register with such purpose would exist.
      However, we have tiles up to `0xd5f0` into this section of memory, so there has to be some specifier somewhere.
All tiles are 4bpp, and most pixel values seem to be either 0 or between `d` and `f`.
This is correct, the 4 colors they use are at the end of the 16 entry "bank" starting at `1c0`, or, if the button
is selected, starting at `1e0`.
The number of the palette bank should therefore be either 14 or 15 (`e` or `f`), depending on if the button is selected or not.

We can also look at the 7 selectors on the top of the screen. These have tile entries
```
... 29 10  29 12  39 20  39 22  29 30  29 32  29 40  etc.
```
Tiles start at `0x2200` into the tile data portion of VRAM.
This corresponds with IDs of `0x110` and up, which makes sense, and corresponds with the 9 bit tile IDs above.
In fact, having 8 or 7 bits of tile ID would yield a different offset than above, and would be quite strange.
The mapping appears to be the same.

Then there is the little puppet in the selection menu. The tiles start at `c000` into the tile data region.
Pixels are all `8 - b`. The palette seems to be missing, I do not know what is up with that.
Tile entries are 
```
... 0e 00  0e 02 ...
... 0e 04  0e 06 ...
etc.
```
If there was no register to specify tile offset (which I suspect at this point), the tile ID would be 11 bits.

Therefore, I assume that the tile ID is 11 bits. Conclusion for now:
```
bits  meaning
0-10  Tile ID (starting from either 0, but probably some register somewhere
11    ??? (1 for all Animeland tiles, could have something to do with the mapping)
12-13 Palette, starting from palette offset specified in some IO register
14-15 ??? (00 for all animeland tiles, could have something to do with the mapping?)
```