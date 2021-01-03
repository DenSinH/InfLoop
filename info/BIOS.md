# Known BIOS calls:
 - 668 (Animeland, almost first thing, boot screen?)
 - 2f74 A simple memcpy16
 - 3ef0 Another simple memcpy16, but now in a weird swizzled order for visual effect (Animeland face appearing effect)?
 - 6644 (Animeland, always called before Kana unpack)
 - 5f4c (1bpp 16x16 to 4BPP 16x16 2D tiles with offset, kana unpacking routine)
 - 6ac0 (Animeland, setup?)
 - 6b50 (Animeland, setup?)
 - 613c (Animeland, setup?)
 - 437c Decompression (4 bit)
 - 445c Decompression (8 bit)
 - 6028 1BPP 16x16 to 4BPP 2D mapping unpack
 - 60a4 4x4 2BPP tile unpacking
 - 66d0: memory copy / replace with DMA
 - 6a0e: same as before, but without DMA
 - 6a48: same as before (I really don't know what the difference is here)
 - 7d96: Unpack TileMap

### 6028

1BPP 16x16 tiles to 4BPP 16x16 2D mapped tiles unpacking call. 
```
r4: src, pointer to tile data
r5: dest, destination pointer
r6: count
r7: some pointer? maybe some usable buffer for the function to use while unpacking
```
Unpacks `count` 1BPP 16x16 tiles from `src` to `dest` (1 -> 4BPP), much like 5f4c, but repeated. 
Also accounts for the offset in each row of the 2D mapping this way.

### 60a4

2BPP tile unpacking call
```
r4: pointer to tile data
r5: destination pointer
r6: offset
r7: some pointer? maybe some usable buffer for the function to use while unpacking
```
Unpacks 16 tiles (4x4 set of tiles) from `src` to `dest` with an offset `offset` added to each pixel.
 
### 5f4c

1BPP 16x16 tiles (Kana) unpacking call. Unpacks tiles/kana packed as 1BPP tiles to 4BPP tiles in the 2D mapping
that the PPU uses.
```
r4: pointer to tile data
r5: destination pointer
r6: offset
r7: pointer to some struct set up by 6644?
```
Basically the same as the previous but with 2 tiles. Although this call might not be complete yet.

### 445c

Bitmap decompression call. 
```
r4: ptr to uncomp data
``````C
struct BitmapUncompData {
    u16 width;
    u16 height;
    struct header* header;
    void* data;
}
```
where 
```C
struct  header {
    u8 table_length;
    u8 constant;  // I don't know the meaning of this
    u8 data[table_length];
}
```
I don't know the compression used.

### 443c
This call is the same as `445c`, except the data is 4 bits. The table holds the nibbles in bytes,
but all top nibbles are 0.

### 66d0, 6a0e and 6a48

These calls do virtually the same thing, except `66d0` can use a DMA channel specified as second argument.
The first argument is a pointer to a stream of structs and ushorts. I viewed the structs as:
```C
struct {
    u16 flags;
    u16 _padding;
    void* src;
    u16 mask;  // so for example f800: mem = (mem & mask) | value
    u16 value;
    u16 len;   // units, not bytes
    u16 _padding;
};
```
OR
```C
struct {
    u16 flags;
    u16 _padding;
    void* src;
    void* dest;
    u16 len;   // units, not bytes
    u16 _padding;
};
```
depending on the setting of the flags. The `_padding` is only there because the `flags` field is not actually 
part of the struct. I have seen the following settings for the flags:
    - 0, 1, 6: Memcpy, use second struct, straight copy of `len` words from src to dest. There has to be some difference
       between the different flags, but I don't know this yet.
    - 5: use first struct, set `len` words at `src` to `(word & mask) | value`
    - ffff: termination flag (stop processing the call)
Basically, the call will keep reading structs and flags and copying/setting accordingly.

### 7d96
Unpacks data to tile map. Dream change calls this with a pointer to an array, and a pointer to a location on the stack,
which is then memcpied into the first 700h bytes of VRAM. The tile data for the casio logo is also unpacked. I think the data:
```
                             BYTE_ARRAY_0e090464                             XREF[2]:     SetupCasioScreen:0e08df68(*), 
                                                                                          0e08dfc8(*)  
        0e090464 07 00 00        db[296]
                 3c 00 60 
                 09 29 49 
           0e090464 [0]            07h, 00h, 00h, 3Ch
           0e090468 [4]            00h, 60h, 09h, 29h
           0e09046c [8]            49h, 78h, 04h, 11h
           0e090470 [12]           00h, 01h, 02h, 03h
           0e090474 [16]           05h, 06h, 07h, 08h
           0e090478 [20]           09h, 0Ah, 00h, 2Fh
           0e09047c [24]           00h, 2Fh, 11h, AFh
           0e090480 [28]           04h, 6Fh, 00h, 2Fh
           0e090484 [32]           6Bh, 39h, B9h, 07h
           0e090488 [36]           E3h, 00h, 0Bh, A9h
           0e09048c [40]           04h, 64h, 07h, E1h
           0e090490 [44]           79h, 04h, 64h, 07h
           0e090494 [48]           E1h, 3Bh, 11h, 60h
           0e090498 [52]           07h, E4h, 06h, 74h
           0e09049c [56]           07h, E1h, 06h, A0h
           0e0904a0 [60]           19h, 07h, E4h, 04h
           0e0904a4 [64]           64h, 07h, E1h, 8Bh
           0e0904a8 [68]           99h, 69h, 3Ch, BFh
           0e0904ac [72]           4Ch, FFh, 1Dh, 2Fh
           0e0904b0 [76]           8Dh, 7Fh, 70h, 3Fh
           0e0904b4 [80]           C0h, FFh, 12h, 13h
           0e0904b8 [84]           14h, 15h, 16h, 37h
           0e0904bc [88]           38h, 39h, 3Ah, 3Bh
           0e0904c0 [92]           5Fh, 17h, 18h, 19h
           0e0904c4 [96]           1Ah, 1Bh, 1Ch, 1Dh
           0e0904c8 [100]          3Ch, 3Dh, 3Eh, 3Fh
           0e0904cc [104]          40h, 41h, 42h, 43h
           0e0904d0 [108]          60h, 61h, 1Eh, 1Fh
           0e0904d4 [112]          20h, 21h, 22h, 23h
           0e0904d8 [116]          24h, 44h, 45h, 46h
           0e0904dc [120]          47h, 48h, 49h, 4Ah
           0e0904e0 [124]          4Bh, 62h, 63h, 25h
           0e0904e4 [128]          26h, 27h, 28h, 29h
           0e0904e8 [132]          2Ah, 2Bh, 4Ch, 4Dh
           0e0904ec [136]          4Eh, 4Fh, 50h, 51h
           0e0904f0 [140]          64h, 65h, 0Bh, 0Ch
           0e0904f4 [144]          0Dh, 0Eh, 2Ch, 2Dh
           0e0904f8 [148]          2Eh, 2Fh, 30h, 52h
           0e0904fc [152]          53h, 54h, 55h, 56h
           0e090500 [156]          57h, 66h, 67h, 68h
           0e090504 [160]          0Fh, 10h, 31h, 32h
           0e090508 [164]          33h, 34h, 35h, 36h
           0e09050c [168]          58h, 59h, 5Ah, 5Bh
           0e090510 [172]          5Ch, 5Dh, 5Eh, 69h
           0e090514 [176]          6Ah, 6Bh, 97h, DBh
           0e090518 [180]          7Eh, BFh, DFh, 6Fh
           0e09051c [184]          EEh, 97h, C7h, E5h
           0e090520 [188]          79h, 90h, 8Bh, 8Bh
           0e090524 [192]          5Ch, 9Ch, 9Ch, 5Ch
           0e090528 [196]          1Ch, 75h, 36h, E8h
           0e09052c [200]          95h, 8Bh, 8Bh, 8Bh
           0e090530 [204]          8Bh, 8Bh, 5Ch, 5Ch
           0e090534 [208]          5Ch, 5Ch, 5Ch, 5Ch
           0e090538 [212]          5Ch, 5Ch, CCh, C5h
           0e09053c [216]          EEh, 7Dh, F9h, 1Ch
           0e090540 [220]          BFh, 17h, 17h, 17h
           0e090544 [224]          17h, 17h, 17h, B9h
           0e090548 [228]          B8h, B8h, B8h, B8h
           0e09054c [232]          B8h, B8h, B8h, 98h
           0e090550 [236]          8Bh, DDh, EFh, F2h
           0e090554 [240]          CFh, C5h, C5h, C5h
           0e090558 [244]          C5h, 5Ch, 5Ch, 5Ch
           0e09055c [248]          5Ch, 5Ch, CCh, C5h
           0e090560 [252]          5Ch, ECh, 5Ch, 5Ch
           0e090564 [256]          5Ch, 4Ch, 2Eh, D7h
           0e090568 [260]          8Bh, 8Bh, 8Bh, 8Bh
           0e09056c [264]          8Bh, 8Bh, 8Bh, B9h
           0e090570 [268]          98h, 8Bh, 8Bh, 8Dh
           0e090574 [272]          8Bh, CBh, F5h, E0h
           0e090578 [276]          E2h, 22h, 17h, 17h
           0e09057c [280]          17h, 17h, 17h, 17h
           0e090580 [284]          17h, 17h, E9h, ECh
           0e090584 [288]          E2h, 62h, BFh, DFh
           0e090588 [292]          EFh, F7h, 4Bh, 00h
```
(296, or 0x128) bytes, must be unpacked into the 0x700 bytes making up the casio screen tile map.
First word: unpacked size. Last byte might be padding. Take a look at the following sequence of bytes:
```
 0e0904f8 [148]          2Eh, 2Fh, 30h, 52h
 0e0904fc [152]          53h, 54h, 55h, 56h
 0e090500 [156]          57h, 66h, 67h, 68h
 0e090504 [160]          0Fh, 10h, 31h, 32h
 0e090508 [164]          33h, 34h, 35h, 36h
 0e09050c [168]          58h, 59h, 5Ah, 5Bh
 0e090510 [172]          5Ch, 5Dh, 5Eh, 69h
 0e090514 [176]          6Ah, 6Bh,         
```
Taking a look at the tile map that Animeland so graciously provides for us, we see that these values appear in this order, 
except interrupted by some values. Also note that everything past the byte `6bh`, at `178` is very very likely no longer part of the tile map, 
as the second bytes of the screen entries only go up to `6bh`. The values after this point seem to be very repetitive.
Note that `5c = ROTR(b8, 1) = ROTL(17, 2)`. It seems to definitely be a bit stream, with common values `0101 1100`. 
I suspect this encodes the values `04 00`, as they are the most common. 

The numbers between index 82 and 178 all occur once (I think), and are in the "table" in order of first occurrence.
Now `178 - 82 = 96 = 0x60`. 82 starts `3c` entries after the initial sequence of incrementing values ending in `0a`. 
