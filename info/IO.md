# IO

What IO would be used for:
 - Video
 - Audio
 - Sticker printer addon
 - SIO/Controller

There seem to be a lot of IO regions in the 0C0X'XXXX part of the address space:
 - 0C04'XXXX: VRAM
 - 0C05'1XXX: PRAM
 - 0C05'8XXX: ???
 - 0C05'9XXX: Seems to hold 4 sets of identical IO registers, spaced 8 bytes apart
              Animeland has a function to set these IO registers, (FUN_0e003104 calls it for all regions, based on some status)
              Function is at 0e014638. There seem to be 4 sets of 6 registers
              Then 2 more registers for general settings (040 and 050)
              Animeland has an array of structs/u16 arrays holding the data for these registers
              This array is at 0e01912a.
              This array seems to be of length 5, 4 elements for settings in one case,
              1 element in another case
 - 0C05'AXXX: ???
 - 0C05'BXXX: ???
 - 0C05'DXXX: ???
 - 0C05'F000: seem to be 80h ushorts of IO (cleared to 0 by Animeland)