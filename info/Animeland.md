# Animeland

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