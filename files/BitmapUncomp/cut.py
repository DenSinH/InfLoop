with open("../roms/Animeland.bin", "rb") as f:
    data = f.read()

with open("./comp.bin", "wb+") as f:
    # overestimate the size
    # uncompressed size should be 0xe000 bytes
    # header is 0x52 bytes
    # compressed should be at most this big
    f.write(data[0x1f566c:0x1f566c + 0x52 + 0xe000])