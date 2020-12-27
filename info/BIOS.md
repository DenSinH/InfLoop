# Known BIOS calls:
 - 668 (Animeland, almost first thing, boot screen?)
 - 6644 (Animeland, always called before Kana unpack)
 - 5f4c (2bpp kana unpacking routine)
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

### 6628

2BPP tile unpacking call. 
```
r4: src, pointer to tile data
r5: dest, destination pointer
r6: count
r7: some pointer? maybe some usable buffer for the function to use while unpacking
```
Unpacks `count` 2BPP tiles from `src` to `dest` (2 -> 4BPP).

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