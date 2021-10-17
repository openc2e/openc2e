# codec: utf-8

import struct
import sys

def read_u16le(f):
    return struct.unpack("<H", f.read(2))[0]

def read_u32le(f):
    return struct.unpack("<I", f.read(4))[0]

def read_cstring(f):
    size = f.read(1)[0]
    s = f.read(size)
    assert len(s) == size
    return s

def decode_creaturesstring(b):
    num_ascii = 0
    for c in b:
        if c < 128:
            num_ascii += 1

    if num_ascii * 2 > len(b):
        # if more than half ascii, likely CP1252
        try:
            return b.decode('cp1252')
        except:
            return b
    else:
        # if not more than half ascii, try cp932
        # TODO: better heuristics? Like, creatures strings in Japanese have a
        # lot of double-byte katakana characters which should be recognizable.
        try:
            return b.decode('cp932')
        except:
            return b

def main():
    if len(sys.argv) != 2:
        sys.stderr.write("USAGE: {} filename".format(sys.argv[0]))
    
    filename = sys.argv[1]
    
    with open(filename, "rb") as f:
        num_items = read_u16le(f)
        print(f"{num_items=}")
        for i in range(num_items):
            print(f"item {i+1}")
            remaining_qty = read_u16le(f)
            print(f"{remaining_qty=}")
            script = read_cstring(f)
            print(f"{script=}")
            picture_width = read_u32le(f)
            print(f"{picture_width=}")
            picture_height = read_u32le(f)
            print(f"{picture_height=}")
            picture_width2 = read_u16le(f)
            print(f"{picture_width2=}")
            assert picture_width == picture_width2
            imagedata = f.read(picture_width * picture_height)
            image_name = decode_creaturesstring(read_cstring(f))
            print(f"{image_name=}")
            image_description = decode_creaturesstring(read_cstring(f))
            print(f"{image_description=}")

if __name__ == '__main__':
    main()