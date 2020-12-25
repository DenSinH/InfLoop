typedef unsigned char   undefined;

typedef unsigned char    bool;
typedef unsigned char    byte;
typedef unsigned int    uint;
typedef unsigned char    undefined1;
typedef unsigned short    undefined2;
typedef unsigned int    undefined4;
typedef unsigned short    ushort;

struct s_BitmapDecomp {
    ushort width;
    ushort height;
    void* header;
    void* data;
};

struct s_Tile4BPPDecomp {
    ushort width;  // in nibbles
    ushort height; // in nibbles
    void* header;
    void* data;
};

typedef ushort TileMap[0x380];
typedef byte TileData4BPP[32];

struct TileData {
    ushort OBJFlags;
    ushort padding;
    byte data[256];
};

struct s_TileData4BPPWithColors {
    ushort colors[4];
    TileData4BPP data[16];
};

struct IOC0590Settings {
    ushort reg0;
    ushort reg1;
    ushort reg2;
    ushort reg3;
    ushort reg4;
    ushort reg5;
    ushort reg6;
    ushort reg7;
    ushort reg8;
    ushort reg9;
};

struct OBJDataUnpackResult { /* Used in Copy4BPPTileData at around 0e003b10 for a BIOS call */
    uint field_0x0;
    uint finished;
    char data[7][512];
    ushort OBJFlags1[7];
    ushort padding0;
    ushort OBJFlags2[7];
    ushort padding1;
};

struct Struct50h {
    byte field_0x0[76];
    void * pointer;
};