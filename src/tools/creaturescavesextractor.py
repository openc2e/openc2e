# codec: utf-8

import re
import os
import sys
import shutil
import tempfile
import libarchive.public

def natural_sort(l): 
    convert = lambda text: int(text) if text.isdigit() else text.lower()
    alphanum_key = lambda key: [convert(c) for c in re.split('([0-9]+)', key)]
    return sorted(l, key=alphanum_key)

def main():
    if len(sys.argv) == 1:
        sys.stderr.write(f"Usage: {sys.argv[0]} FILENAMES...\n")
        exit(1)
    
    for filename in natural_sort(sys.argv[1:]):
        assert filename.lower().endswith(".zip")
        
        output_dir, ext = os.path.splitext(filename)
        base_output_name = os.path.basename(output_dir)
        
        if os.path.exists(output_dir):
            continue
            
        print(f"Extracting {filename}...")
        
        with tempfile.TemporaryDirectory() as tempdir:
            with open(filename, "rb") as f:
                if f.peek() == b'':
                    # empty file??
                    os.makedirs(output_dir, exist_ok=True)
                    continue
                
                magic = f.peek()
                if magic.startswith(b"PK\x03\x04") or magic.startswith(b"PK\x05\x06") or magic.startswith(b"PK00PK\x03\x04") or magic.startswith(b"Rar!") or magic.startswith(b"7z\xbc\xaf"):
                    # archives
                    try:
                        with libarchive.public.file_reader(filename) as ar:
                            for entry in ar:
                                if not entry.filetype.IFREG:
                                    continue
                                
                                output_name = os.path.join(output_dir, entry.pathname)
                                print(output_name)
                                
                                temp_name = os.path.join(tempdir, entry.pathname)
                                os.makedirs(os.path.dirname(temp_name), exist_ok=True)
                                with open(temp_name, "wb") as f:
                                        for block in entry.get_blocks():
                                            f.write(block)
                        os.rename(tempdir, output_dir)
                    except Exception as ex:
                        sys.stderr.write(f"Error extracting {filename} - try an external program\n")
                        sys.stderr.write(f"Error: {ex}\n")
                        
                        if magic.startswith(b"Rar!"):
                            shutil.copy(filename, os.path.join(tempdir, base_output_name + ".rar"))
                            os.rename(tempdir, output_dir)
                elif magic.startswith(b"PRAYGLST"):
                    glst_name = magic[8:136]
                    assert len(glst_name) == 128
                    glst_name = glst_name[:glst_name.find(b'\x00')].decode('cp1252')
                    if glst_name.endswith(".DSEX.glist.creature"):
                        filename = re.sub(r"\.DSEX\.glist\.creature$", ".ds.creature", glst_name)
                    elif glst_name.endswith(".glist.creature"):
                        filename = re.sub(r"\.glist\.creature$", ".creature", glst_name)
                    else:
                        raise NotImplementedError(glst_name)
                    
                    output_name = os.path.join(output_dir, filename)
                    print(output_name)
                    
                    temp_name = os.path.join(tempdir, filename)
                    os.makedirs(os.path.dirname(temp_name), exist_ok=True)
                    with open(temp_name, "wb") as out:
                        out.write(f.read())
                    
                    os.rename(tempdir, output_dir)
                elif magic.startswith(b"\xff\xff\x01\x00\x08\x00Creature"):
                    # creatures 1 .exp
                    filename = base_output_name + ".c1.exp"
                    output_name = os.path.join(output_dir, filename)
                    print(output_name)
                    
                    temp_name = os.path.join(tempdir, filename)
                    os.makedirs(os.path.dirname(temp_name), exist_ok=True)
                    with open(temp_name, "wb") as out:
                        out.write(f.read())
                    
                    os.rename(tempdir, output_dir)
                elif magic.startswith(b"\xff\xff\x02\x00\x08\x00Creature"):
                    # creatures 2 .exp
                    filename = base_output_name + ".c2.exp"
                    output_name = os.path.join(output_dir, filename)
                    print(output_name)
                    
                    temp_name = os.path.join(tempdir, filename)
                    os.makedirs(os.path.dirname(temp_name), exist_ok=True)
                    with open(temp_name, "wb") as out:
                        out.write(f.read())
                    
                    os.rename(tempdir, output_dir)
                elif magic.startswith(b"\xff") and magic[6:10] == b'JFIF':
                    # jpeg file???
                    filename = base_output_name + ".jpeg"
                    output_name = os.path.join(output_dir, filename)
                    print(output_name)
                    
                    temp_name = os.path.join(tempdir, filename)
                    os.makedirs(os.path.dirname(temp_name), exist_ok=True)
                    with open(temp_name, "wb") as out:
                        out.write(f.read())
                    
                    os.rename(tempdir, output_dir)
                elif magic.startswith(b"<!DOCTYPE HTML"):
                    # probably missing file
                    filename = base_output_name + ".html"
                    output_name = os.path.join(output_dir, filename)
                    print(output_name)
                    
                    temp_name = os.path.join(tempdir, filename)
                    os.makedirs(os.path.dirname(temp_name), exist_ok=True)
                    with open(temp_name, "wb") as out:
                        out.write(f.read())
                    
                    os.rename(tempdir, output_dir)
                else:
                    raise NotImplementedError(magic)

if __name__ == '__main__':
    main()