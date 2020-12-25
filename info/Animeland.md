#Animeland

Interesting stuff found in the Animeland ROM:
 - Function at 0e000884 to VSync and check some sort of video status
 - Array at 0e0d5cc4 holding pointers to structs of the form
   ```CXX
   struct {
       u32 SomeFlag; // this value always seems to be 3D160000
       u8 Data[0x100];
   };
   ```
   followed by a huge array of those data structs pointed to by the array
 - FUN_0e00b118: SVC call with args:
    `(void*)09001008`, `0xff`, `0x61`, `u8** -> 0e0b2a94`
   At `0e0b2a94` seems to be an array of pointers to byte streams. My suspicion is that
   these are part of some sort of decompression routine. Arguments to the function would be
   a destination pointer, ???, index into the `u8**`, and the `u8**`.
   It is worth investigating if other games do a similar thing, and what sort of compression
   is used on the byte stream. 
   
   
##ROM layout:
 - `0x0e00'0000` - `0x0e00'0480`: Header + VBR table
 - `0x0e00'0480` - `0x0e01'70f0`: Code
 - `0x0e01'70F0` - `0x0e01'f948`: Data (arrays, pointers, structs)
 - `0x0e01'f948` - `0x0e02'1d00`: FF filled region
 - `0x0e02'1d00` - `0x0e02'1db0`: ???
 - `0x0e02'1db0` - `0x0e02'3810`: Kana 2bpp linear tile data
 - `0x0e02'3810` - `0x0e02'4c20`: More 2bpp linear tile data
 - `0x0e02'4c20` - `0x0e02'6d60`: ???
 - `0x0e02'4c20` - `0x0e03'8f60`: 4bpp data for dancing girl tiles
 - `0x0e03'8f60` - `0x0e03'95a8`: pointers to (compressed?) data streams and data streams
 - `0x0e03'95a8` - `0x0e05'2ba8`: 58 tile maps? notice all the 4's everywhere?
 - `0x0e05'2ba8` - `0x0e0b'0ff4`: Compressed tiles to be unpacked with BIOS 437c
 - `0x0e0b'0ff4` - `0x0e0b'2a94`: 4bpp tile data
 - `0x0e0b'2a94` - `0x0e0c'fe03`: table of pointers to byte streams (could this be text or compressed data streams?)
 - `0x0e0c'fe04` - `0x0e0d'5cc3`: table of pointers to byte streams (compressed data? (I suspect RLE) or bytecode/instructions for the printer. I see the value 7E 00 FF FF) a lot
 - `0x0e0d'5cc3` - `0x0e10'a260`: Pointer array to tile data, then an array of tile data as
 ```C
struct {
    u32 ID = 3D160000;
    u8 data[0x100];  // 2BPP data
};
```
 - `0x0e10'a260` - `0x0e10'a330`: Array of pointers to char*s terminated by FF
 - `0x0e10'a334` - `0x0e10'a378`: Array of pointers to 6 byte structs (u16, u16, u16?)
 - `0x0e10'a378` - `0x0e10'd32c`: Array of pointers to byte streams. These byte streams all seem to end in FF (strings?)
 - `0x0e10'd32c` - `0x0e10'd3dc`: char streams pointed to by above (-a330)
 - `0x0e10'd3dc` - `0x0e11'2478`: byte streams
 - `0x0e11'2478` - `0x0e11'6d68`: Array of 4BPP linear tiles
 - `0x0e11'6d98` - `0x0e11'716c`: Array of pointers to structs (about 104h in size) (palettes?)
 - `0x0e11'716c` - `0x0e12'5da8`: Array of structs (about 104h in size) (palettes?)
 
## VBR Table

All entries are a BIOS function (not sure if it's really a useful one, it might just not expect interrupts).
Except the entry at offset 120h, which is the DMAC0/1 priority 3 interrupt.
The handler pushes all registers onto the stack, and calls 6238h as
`(sub_6238h)(0x0900'1008, 0xff)`
then it pops every single register again and returns.
    
 