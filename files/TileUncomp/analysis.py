with open("comp.bin", "rb") as f:
    data = f.read()

# this is the structure of the data for the BIOS call
header_len   = data[0] + 1
header_byte1 = data[1]
header = data[2:2 + header_len]
stream = data[2 + header_len:]

"""
header data:
len   5 + 1
byte1 3 # LZ offset?

0x1
0x7
0xa
0x2
0x5
0x0

Logo should look like in the html file.
Looking at the palette, all entries should be 0 for black (transparent?) 1 for background, 2 for white, then increasing values for less bright stuff
Judging from the table values, only entries 0/1, 2 and less brighter ones (5, 7, a) will be used.
The values in the tile map suggest that the tiles are in here in increasing order.

1 and 2 should definitely be the most common values though.

The first tile seems to be one third/one half blue first, so about 0x15-0x21 bytes that are either 0 or 1
"""

for i in stream:
    print("%02x" % i)
