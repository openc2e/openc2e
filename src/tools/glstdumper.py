# codec: utf-8

import sys
import struct
import zlib
import string

class cursor:
    def __init__(self, buf):
        self._buf = buf
        self._p = 0

    def read(self, n):
        val = self._buf[self._p : self._p + n]
        assert(len(val) == n)
        self._p += n
        return val
    
    def peek(self, n=None):
        if n is None:
            return self._buf[self._p:]
        val = self._buf[self._p : self._p + n]
        assert(len(val) == n)
        return val
    
    def skip(self, n):
        self._p += n

    def read_u32le(self):
        return struct.unpack("<I", self.read(4))[0]

    def read_s32le(self):
        return struct.unpack("<i", self.read(4))[0]

    def read_string(self):
        length = self.read_s32le()
        return self.read(length)

def main():
    if len(sys.argv) not in (2, 3):
        sys.stderr.write("USAGE: {} FILE [OUTPUT]\n".format(sys.argv[0]))
        sys.exit(1)
    filename = sys.argv[1]

    with open(filename, "rb") as f:
        data = f.read()
    
    magic = b"Creatures Evolution Engine - Archived information file. zLib 1.13 compressed.\x1a\x04"
    if data.startswith(magic):
        data = zlib.decompress(data[len(magic):])
    
    cur = cursor(data)
    
    file_version = cur.read_u32le()
    assert file_version in (0xc, 0x27)
    
    unknown = cur.read_u32le()
    assert unknown == 1
    
    moniker = cur.read_string()
    print(f"{moniker=}")
    
    moniker = cur.read_string()
    print(f"{moniker=}")
    
    name = cur.read_string()
    print(f"{name=}")
    
    gender = cur.read_u32le()
    print(f"{gender=}")
    
    genus = cur.read_u32le()
    print(f"{genus=}")
    
    species = cur.read_u32le()
    print(f"{species=}")
    
    num_events = cur.read_u32le()
    print(f"{num_events=}")

    for _ in range(num_events):
        print("event:")
        
        for n in ("event_no", "world_time"):
            val = cur.read_u32le()
            print(f"  {n}={val}")
        
        for n in ("creature_age",):
            val = cur.read_s32le()
            print(f"  {n}={val}")
        
        for n in ("unix timestamp",):
            val = cur.read_u32le()
            print(f"  {n}={val}")
        
        for n in ("life stage",):
            val = cur.read_s32le()
            print(f"  {n}={val}")
        
        for n in ("moniker1", "moniker2", "user text", "phot block", "world name", "world id"):
            val = cur.read_string()
            print(f"  {n}={val}")
        
    for n in ("point mutations", "crossover"):
        val = cur.read_u32le()
        print(f"{n}={val}")
    
    assert not cur.peek()


if __name__ == "__main__":
    main()
