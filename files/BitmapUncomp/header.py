from pprint import pprint
from PIL import Image
import numpy as np

with open("comp.bin", "rb") as f:
    data = f.read()

header = data[:0x4e]
image = data[0x4e:]

"""
0x4c # header length
0x2  # might signify that th huffman tree ends in trees of size 2???

       https://users.dcc.uchile.cl/~gnavarro/ps/dcc13.2.pdf


EVEN ADDRESS
0x80
0x5
0x88
0x8
0xe
0x19
0x16
0x1f
0x24
0x4
0x7
0xd
0x18
0x15
0x1e
0x23
0x21
0xc
0x17
0x1d
0x20
0x2
0x3
0x6
0xb
0x14
0xa
0x9
0x1c
0x1a
0x1b
0x81
0x13
0x12
0x11
0xf
0x10
0x82
0x2a
0x2b
0x28
0x27
0x29
0x26
0x74
0x75
0x7d
0x7a
0x73
0x72
0x71
0x70
0x7b
0x7f
0x7c
0x83
0xa0
0x77
0xa1
0xa2
0xa3
0xa4
0xa5
0x7e
0x1
0x30
0x2f
0x2e
0x2d
0x2c
0x84
0x85
0x76
0x86
0x87
0xa7
"""

table = header[2:]
print(len(set(table)), len(table))
# i = 0
# ones = 0
# cipher = {}
# while i < len(table) & ~7:
#     key = (1 << ones) - 1
#     cipher[hex((key << 4) | 0b000)] = hex(table[i])
#     cipher[hex((key << 4) | 0b001)] = hex(table[i + 1])
#     cipher[hex((key << 4) | 0b010)] = hex(table[i + 2])
#     cipher[hex((key << 4) | 0b011)] = hex(table[i + 3])
#     cipher[hex((key << 4) | 0b100)] = hex(table[i + 4])
#     cipher[hex((key << 4) | 0b101)] = hex(table[i + 5])
#     cipher[hex((key << 4) | 0b110)] = hex(table[i + 6])
#     cipher[hex((key << 4) | 0b111)] = hex(table[i + 7])
#     i += 4
#     ones += 1
#
# pprint(cipher)

image = list(image)
w = 256
h = 112
img = np.zeros((h, w), dtype=np.uint8)
for i in range(h):
    print(i, hex(w * i), image[w * i: w * (i + 1)])
    img[i, :] = np.array(image[w * i:w * (i + 1)])

Image.fromarray(img, mode="L").save("test.png")