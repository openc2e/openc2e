# codec: utf-8

import sys
import struct
import string


def main():
    if len(sys.argv) not in (2, 3):
        sys.stderr.write(f"USAGE: {sys.argv[0]} FILE [OUTPUT]\n")
        sys.exit(1)
    filename = sys.argv[1]
    output_filename = sys.argv[2] if len(sys.argv) == 3 else None

    with open(filename, "rb") as f:
        magic = f.read(8)
        assert magic == b"SZDD\x88\xf0\x27\x33"
        compression_mode = f.read(1)
        assert compression_mode == b"A"
        missing_filename_character = f.read(1)
        assert missing_filename_character in (
            b"\x00" + (string.ascii_letters + string.digits).encode("ascii")
        )
        unpacked_length = struct.unpack("<I", f.read(4))[0]

        out = b""

        window = []
        for _ in range(4096):
            window.append(" ")

        pos = 4096 - 16

        while True:
            if not f.peek():
                break
            control = struct.unpack("<B", f.read(1))[0]
            cbit = 1
            while cbit & 0xFF:
                if control & cbit:
                    window[pos] = f.read(1)
                    out += window[pos]
                    pos += 1
                    pos &= 4095  # maybe
                else:
                    if not f.peek():
                        break
                    matchpos = struct.unpack("<B", f.read(1))[0]
                    matchlen = struct.unpack("<B", f.read(1))[0]
                    matchpos |= (matchlen & 0xF0) << 4
                    matchlen = (matchlen & 0x0F) + 3
                    matchpos &= 4095  # maybe
                    for _ in range(matchlen):
                        window[pos] = window[matchpos]
                        out += window[pos]
                        pos += 1
                        matchpos += 1
                        pos &= 4095
                        matchpos &= 4095

                cbit <<= 1

        assert len(out) == unpacked_length

    if output_filename:
        with open(output_filename, "wb") as f:
            f.write(out)
    else:
        print(out)


if __name__ == "__main__":
    main()
