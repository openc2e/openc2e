# codec: utf-8

import sys
import struct
import string

def read_exact(f, n):
    result = f.read(n)
    assert len(result) == n
    return result

def read_u16le(f):
    return struct.unpack("<H", read_exact(f, 2))[0]

def read_u32le(f):
    return struct.unpack("<I", read_exact(f, 4))[0]

def parse_rtf(s):

    out = ''
    
    p = 0
    while p < len(s):
        if s[p] == '{':
            p += 1
            
            if s[p:].startswith('\\fonttbl'):
                p += len('\\fonttbl')
                num_groups = 0
                while p < len(s):
                    if s[p] == '}' and num_groups == 0:
                        break
                    if s[p] == '}':
                        num_groups -= 1
                    if s[p] == '{':
                        num_groups += 1
                    p += 1
            elif s[p:].startswith('\\colortbl'):
                p += len('\\colortbl')
                num_groups = 0
                while p < len(s):
                    if s[p] == '}' and num_groups == 0:
                        break
                    if s[p] == '}':
                        num_groups -= 1
                    if s[p] == '{':
                        num_groups += 1
                    p += 1
            elif s[p:].startswith('\\*'):
                p += len('\\*')
                num_groups = 0
                while p < len(s):
                    if s[p] == '}' and num_groups == 0:
                        break
                    if s[p] == '}':
                        num_groups -= 1
                    if s[p] == '{':
                        num_groups += 1
                    p += 1
            elif s[p:].startswith('\\pntext'):
                p += len('\\pntext')
                num_groups = 0
                while p < len(s):
                    if s[p] == '}' and num_groups == 0:
                        break
                    if s[p] == '}':
                        num_groups -= 1
                    if s[p] == '{':
                        num_groups += 1
                    p += 1
        elif s[p] == '}':
            p += 1
        elif s[p] == '\\':
            command = ''
            p += 1
            while p < len(s) and s[p] in (string.ascii_letters + string.digits + '-'):
                command += s[p]
                p += 1
            if command == 'par':
                out += '\n'
        elif s[p] in ('\r', '\n'):
            # ignore
            p += 1
        else:
            out += s[p]
            p += 1
    
    out = out.strip()
    return "\n".join(line.strip() for line in out.split("\n"))

class GeneDescription:
    def read(self, f):
        self.type = read_u16le(f)
        self.subtype = read_u16le(f)
        self.sequence_number = read_u32le(f)
        
        description_size = read_u16le(f)
        self.description = read_exact(f, description_size).decode('ascii')
        
        comments_size = read_u16le(f)
        self.comments = parse_rtf(read_exact(f, comments_size).decode('ascii'))
        
        print(vars(self))

class GnoFile:
    def read(self, f):
        self.unknown = read_u16le(f)
        assert self.unknown == 2
        
        num_genes = read_u16le(f)
        
        for _ in range(num_genes):
            g = GeneDescription()
            g.read(f)
            
        print(num_genes)
        print(len(f.read()))

def main():
    if len(sys.argv) != 2:
        sys.stderr.write(f"USAGE: {sys.argv[0]} FILE\n")
        sys.exit(1)
    filename = sys.argv[1]

    with open(filename, "rb") as f:
        g = GnoFile()
        g.read(f)

if __name__ == "__main__":
    main()