# IO

What IO would be used for:
 - Video
 - Audio
 - Sticker printer addon
 - SIO/Controller
 - OAM

There seem to be a lot of IO regions in the 0C0X'XXXX part of the address space:
 - 0C04'XXXX: VRAM
 - 0C05'0XXX: Animeland:
                                 LAB_0e00e148                                    XREF[1]:     0e00e2c8(j)  
            0e00e148 d4 50           mov.l      @(DAT_0e00e28c,pc),r4                            = 0C050000h
            0e00e14a 93 97           mov.w      @(DAT_0e00e27c,pc),r3                            = 5080h
            0e00e14c 24 31           mov.w      r3,@r4=>DAT_0c050000
            0e00e14e 74 02           add        0x2,r4
            0e00e150 63 cd           extu.w     r12,r3
            0e00e152 24 31           mov.w      r3,@r4=>DAT_0c050002
            0e00e154 74 02           add        0x2,r4
            0e00e156 93 92           mov.w      @(DAT_0e00e27e,pc),r3                            = 5280h
            0e00e158 24 31           mov.w      r3,@r4=>DAT_0c050004
            0e00e15a 74 02           add        0x2,r4
            0e00e15c 63 cd           extu.w     r12,r3
            0e00e15e 24 31           mov.w      r3,@r4=>DAT_0c050006
            0e00e160 74 02           add        0x2,r4
            0e00e162 93 8d           mov.w      @(DAT_0e00e280,pc),r3                            = 5480h
            0e00e164 24 31           mov.w      r3,@r4=>DAT_0c050008
            0e00e166 74 02           add        0x2,r4
            0e00e168 63 cd           extu.w     r12,r3
            0e00e16a 24 31           mov.w      r3,@r4=>DAT_0c05000a
            0e00e16c 74 02           add        0x2,r4
            0e00e16e 93 88           mov.w      @(DAT_0e00e282,pc),r3                            = 5680h
            0e00e170 24 31           mov.w      r3,@r4=>DAT_0c05000c
            0e00e172 74 02           add        0x2,r4
            0e00e174 62 43           mov        r4,r2
            0e00e176 74 02           add        0x2,r4
            0e00e178 63 cd           extu.w     r12,r3
            0e00e17a 22 31           mov.w      r3,@r2=>DAT_0c05000e
            0e00e17c 67 6c           extu.b     r6,r7
 - 0C05'1XXX: PRAM
 - 0C05'2XXX: ???
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