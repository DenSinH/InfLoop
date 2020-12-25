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
0x0 (?)

Logo should look like in the html file.
Looking at the palette, all entries should be 0 for black (transparent?) 1 for background, 2 for white, then increasing values for less bright stuff
Judging from the table values, only entries 0?/1, 2 and less brighter ones (5, 7, a) will be used.
The values in the tile map suggest that the tiles are in here in increasing order.

1 and 2 should definitely be the most common values though.

The first tile seems to be one third/one half blue first, so about 0x15-0x21 bytes that are (either 0 or?) 1
"""

for i in stream:
    print("%02x" % i)

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

"""
