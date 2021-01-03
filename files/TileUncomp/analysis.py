with open("comp.bin", "rb") as f:
    data = f.read()

# this is the structure of the data for the BIOS call
header_len   = data[0] + 1
header_byte1 = data[1]
header = data[2:2 + header_len]
stream = data[2 + header_len:]

"""
header data:
len   5 (+ 1?)
byte1 3 # LZ offset?

0x1
0x7
0xa
0x2
0x5
0x0 (not part of the header, stream must be aligned by 2)

Logo should look like in the html file.
Looking at the palette, all entries should be 0 for black (transparent?) 1 for background, 2 for white, then increasing values for less bright stuff
Judging from the table values, only entries 0?/1, 2 and less brighter ones (5, 7, a) will be used.
The values in the tile map suggest that the tiles are in here in increasing order.

1 and 2 should definitely be the most common values though.

The first tile seems to be one third/one half blue first, so about 0x15-0x21 bytes that are (either 0 or?) 1
Suspected first few nibbles of data:
11111111
11111111
11111117
1111a722
11152222
(something like this)
"""

print(hex(len(stream)))

"""
using LZW
02 ed 30 c3 52

000|0 00|10 1|110| 110|1 00|11 0000 1100 0011 0101 0010
000: output table[0] -> (1), remember (000)
000: output table[0] -> (1), set table[5] = (1, 1), remember (101 <code>)
101: output table[5] -> (1, 1), set table[6] = (1, 1, 1), remember (110 <code>)
110: output table[6] -> (1, 1, 1), set table[7] = (1, 1, 1, 1), remember (111 <code>)
110: output table[6] -> (1, 1, 1), set table[8] = (1{5}), remember (1000 <code>)
# by now we have 10 pixels as 1

02 ed 30 c3 52 36 25 0a f6 2f 09
0000 0010  1110 1101  0011 0000  1100 0011  0101 0010  0011 0110  0010 0101  0000 1010  1111 0110  0010 1111  0000 1001

later:      (b1 56 20) b1 36 b1 36  b1 6d 16 d1 6d 16 d1 6d 16
1011 0001 0011 0110 repeated
and 0001 0110 1101  repeated

even later: (67) 07 65 90 76 59 07 65 90 76 59 07 65 90
0000 0111 0110 0101 1001 repeated

16d16d16 is reapeated later as well
so is 765907

(36 90) 36 b1 56 b1 36 b1 36 b1
07 67 07 67 07 67 07 67 07 67 06
19 05 69 05 69 05 69 05 69 05 69 05 69 04

Basically, repeated nibble patterns. I also saw a lot of repeated byte patterns in the bitmap compressed data.
The repeated patterns are of different lengths:
4, 3, 6 nibbles, not all divisible by 3 (the constant in the header).
I suspect the common unit of bit length would therefore be a nibble.

"""
