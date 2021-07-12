import struct
import zlib

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

    def read_unknown_s32le(self, expected):
        if type(expected) not in (tuple, list):
            expected = (expected,)
        unknown = self.read_s32le()
        print("unknown = {} ({:#x})".format(unknown, unknown))
        if unknown not in expected:
            s = ["{} ({:#x})".format(_, _) for _ in expected]
            if len(s) > 1:
                s[-1] = "or " + s[-1]
            expected_string = ', '.join(s)
            raise Exception("Expected {}; but got {} ({:#x}) - next few bytes: {}".format(expected_string, unknown, unknown, self.read(15)))

def find_pray_block(c):
    assert c.read(4) == b'PRAY'
    while c.peek():
        block_type = c.read(4).decode('ascii')
        block_name = c.read(128)
        block_name = block_name[:block_name.find(b'\x00')].decode('cp1252')
        length_compressed = c.read_u32le()
        length_decompressed = c.read_u32le()
        flags = c.read_u32le()
        
        sys.stderr.write(f"{block_type}, {block_name}, {length_compressed}, {length_decompressed}, {flags}\n")
        
        if block_type != 'CREA':
            c.skip(length_compressed)
            continue
        
        data = c.read(length_compressed)
        if flags:
            data = zlib.decompress(data)
            
        return data
    
    raise Exception("Couldn't find CREA block in PRAY file")
    exit()

import sys

with open(sys.argv[1], "rb") as f:
# with open("001-aqua-hgru4-5fevv-aaz32-7npj8.DFAM.creature.out", "rb") as f:
# with open("../SpaceAndAllThatIsOutThere.out", "rb") as f:
    data = f.read()
    

if data[0:4] == b'PRAY':
    cur = cursor(data)
    data = find_pray_block(cur)

magic = b"Creatures Evolution Engine - Archived information file. zLib 1.13 compressed.\x1a\x04"
if data.startswith(magic):
    data = zlib.decompress(data[len(magic):])

if data[0] == 0x27:
    CREATURESARCHIVE_DOCKINGSTATION = True
elif data[0] == 0x0c:
    # creatures3
    CREATURESARCHIVE_DOCKINGSTATION = False
    pass
else:
    raise NotImplementedError(data[0])

# if creatures
# 
# creaturesarchivemagic = cur.read_s32le()
# sys.stderr.write(f"{creaturesarchivemagic=}\n")
# assert(creaturesarchivemagic == 0x27)
# 
# creaturesarchivetype = cur.read_s32le()
# # print("creaturesarchive type", creaturesarchivetype)
# assert(creaturesarchivetype == 0x0)

# cur.read_unknown_s32le(expected=0)
# unknown_zero = cur.read_s32le()
# assert(unknown_zero == 0)
# length = cur.read_s32le()
# name = cur.read(length)
# print(name) # SkeletalCreature
    
import binascii
import html
types = {}

def binrepr(buf):
    out = ''
    for c in buf:
        if c in b'0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~ ':
            out += chr(c)
        else:
            out += '\\x' + bytes([c]).hex()
    return html.escape(out)

print("""
<style type="text/css">

body {
    word-break: break-all;
}

.object-name {
    /* font-size: 0.8em; */
    font-weight: bold;
}

.object {
border: 1px solid;
margin-left: 20px;
}

.object.superclass {
margin-left: 5px;
border-color: rgb(100, 100, 100);
border-style: dashed;
}

.superclass .object-name {
color: rgb(100, 100, 100);
}

.data-length {
font-size: 0.8em;
font-weight: bold;
/* color: rgb(150, 150, 150); */
}

.obst, .oben {
  color: rgb(150, 150, 150);
}

.name {
    font-weight: bold;
    font-size: 0.8em;
}

.note {
    font-weight: bold;
}

.string:before {
display:block;
content:"string";
font-weight: bold;
font-size:0.8em;
}

.unknown:before {
display:block;
content:"unknown";
font-weight: bold;
font-size:0.8em;
}

.string {
    background-color: lightblue;
}

.named-value {
    background-color: lightgreen;
}

.raw {
    color: rgb(150, 150, 150);
}

.data, .raw, .obst, .oben {
    font-family: monospace;
}

</style>
""")

print("<div>")

current_span = b''
i = 0

class printerhelper:
    def __init__(self):
        self.depth = 0
        self.inline = False
    def set_inline(self, inline_):
        self.inline = inline_
        if inline_ == False:
            print("<div></div>")
    def data(self, buf):
        if not buf:
            return
        print("<div {}>".format("style=\"display: inline-block\"" if self.inline else ""))
        print("<div class=\"data-length\">")
        print("{} bytes".format(len(buf)))
        print("</div>")
        print("<div class=\"data\">")
        print("{}".format(binrepr(buf)))
        print("</div>")
        print("</div>")
    def unknown_float(self, buf):
        assert(len(buf) == 4)
        value = round(struct.unpack('<f', buf)[0], 4)
        print("<div class=\"unknown\" {}>".format("style=\"display: inline-block\"" if self.inline else ""))
        print("{} <span class=\"raw\">{}</span>".format(value, binrepr(buf)))
        print("</div>")
    def unknown(self, buf):
        if len(buf) == 4:
            value = struct.unpack('<i', buf)[0]
        elif len(buf) == 2:
            value = struct.unpack('<H', buf)[0]
        elif len(buf) == 1:
            value = struct.unpack('<B', buf)[0]
        else:
            assert(len(buf) in (1, 2, 4))
        print("<div class=\"unknown\" {}>".format("style=\"display: inline-block\"" if self.inline else ""))
        print("{} <span class=\"raw\">{}</span>".format(value, binrepr(buf)))
        print("</div>")
    def string(self, s):
        print("<div class=\"string\"{}><span class=\"raw\">{}</span>{}</div>".format("style=\"display: inline-block\"" if self.inline else "", binrepr(struct.pack('<I', len(s))), html.escape(s)))
    def named(self, name, buf):
        print("<div>")
        print("<span class=\"name\">")
        print("{}".format(name))
        print("</span>")
        print("<span class=\"data-length\">")
        print("{} bytes".format(len(buf)))
        print("</span>")
        print("</div>")
        print("<div class=\"named-value\">")
        print("{}".format(binrepr(buf)))
        print("</div>")
    def signedinteger(self, name, buf):
        if len(buf) == 4:
            value = struct.unpack('<i', buf)[0]
        elif len(buf) == 2:
            value = struct.unpack('<h', buf)[0]
        else:
            assert(len(data) in (2, 4))
        print("<div class=\"named-value\"{}>".format("style=\"display: inline-block\"" if self.inline else ""))
        print("<div class=\"name\">")
        print("{}".format(name))
        print("</div>")
        print("{} <span class=\"raw\">{}</span>".format(value, binrepr(buf)))
        print("</div>")
    def integer(self, name, buf):
        if len(buf) == 4:
            value = struct.unpack('<I', buf)[0]
        elif len(buf) == 2:
            value = struct.unpack('<H', buf)[0]
        elif len(buf) == 1:
            value = struct.unpack('<B', buf)[0]
        else:
            assert(len(data) in (1, 2, 4))
        print("<div class=\"named-value\"{}>".format("style=\"display: inline-block\"" if self.inline else ""))
        print("<div class=\"name\">")
        print("{}".format(name))
        print("</div>")
        print("{} <span class=\"raw\">{}</span>".format(value, binrepr(buf)))
        print("</div>")
    def float(self, name, buf):
        assert(len(buf) == 4)
        value = round(struct.unpack('<f', buf)[0], 4)
        print("<div class=\"named-value\" {}>".format("style=\"display: inline-block\"" if self.inline else ""))
        print("<div class=\"name\">")
        print("{}".format(name))
        print("</div>")
        print("{} <span class=\"raw\">{}</span>".format(value, binrepr(buf)))
        print("</div>")
    def note(self, text):
        print("<div class=\"note\">Note: {}</div>".format(text))

printer = printerhelper()

def parse_caosmachine():
    global i
    
    printer.set_inline(True)
    have_macro_script = struct.unpack("<I", data[i:i+4])[0]
    printer.integer("have macro script?", data[i:i+4])
    i += 4
    for _ in range(17):
        printer.unknown(data[i:i+4])
        i += 4
    printer.set_inline(False)
    
    # printer.data(data[i:i+52])
    # i += 52
    
    if data[i:i+4] == b'\xfe\xff\xff\xff':
        printer.integer("macro script", data[i:i+4])
        i += 4
        assert not have_macro_script
    else:
        start_parse_object('MacroScript')
        parse_macroscript()
        end_parse_object()
    
    num_something = struct.unpack("<I", data[i:i+4])[0]
    printer.integer("num something?", data[i:i+4])
    i += 4
    for _ in range(num_something):
        printer.integer("something", data[i:i+4])
        i += 4
    
    # # if data[i+8:i+23] == b'\x0b\x00\x00\x00MacroScript':
    # if have_macro_script != 0:
    #     start_parse_object('MacroScript')
    #     parse_macroscript()
    #     end_parse_object()
    # 
    # if there's not a macro script, we see \xef\xff\xff\xff and then 12 bytes of zeroes
    # if there's a macro script, we the macro script and then 16 bytes of...
    
    # printer.data(data[i:i+16])
    # i += 16
    
    printer.data(data[i:i+8])
    i += 8
    
    num_vars = struct.unpack("<I", data[i:i+4])[0]
    printer.integer("number of vars", data[i:i+4])
    i += 4
    assert num_vars == 100
    
    for _ in range(100):
        printer.set_inline(True)
        var_type = struct.unpack("<I", data[i:i+4])[0]
        printer.integer("type? 0=int 1=float 2=string", data[i:i+4])
        i += 4
        
        if var_type == 0:
            printer.integer("value", data[i:i+4])
            i += 4
        elif var_type == 1:
            printer.float("value", data[i:i+4])
            i += 4
        elif var_type == 2:
            length = struct.unpack('<I', data[i:i+4])[0]
            s = data[i+4:i+4+length].decode('cp1252')
            i += 4 + length
            printer.string(s)
        elif var_type == 4:
            printer.integer("4 value?", data[i:i+4])
            i += 4
        else:
            raise NotImplementedError(var_type)
            
        printer.unknown(data[i:i+4])
        i += 4
            
        printer.set_inline(False)
    
    length = struct.unpack('<I', data[i:i+4])[0]
    s = data[i+4:i+4+length].decode('cp1252')
    i += 4 + length
    printer.string(s)
    # 
    # printer.data(data[i:i+1112])
    # i += 1112
    # 
    # length = struct.unpack('<I', data[i:i+4])[0]
    # s = data[i+4:i+4+length].decode('cp1252')
    # i += 4 + length
    # printer.string(s)
    # 
    # printer.data(data[i:i+28])
    # i += 28
    # 
    # length = struct.unpack('<I', data[i:i+4])[0]
    # s = data[i+4:i+4+length].decode('cp1252')
    # i += 4 + length
    # printer.string(s)
    
    assert data[i:i+8] == b'\x04\x00\x00\x00OBEN'
        

def parse_limb():
    global i
    # assert data[i+606:i+606+8] == b'\x04\x00\x00\x00OBST'
    
    parse_bodypart()
    
    printer.named("limb attachment data? - 16 rows x 4 columns = 64 bytes", data[i:i+64])
    i += 64
    
    tag = data[i:i+4]
    if tag == b'\xfe\xff\xff\xff':
        printer.signedinteger("next limb", data[i:i+4])
        i += 4
    else:
        start_parse_object("Limb")
        parse_limb()
        end_parse_object()
        
    assert data[i:i+8] == b'\x04\x00\x00\x00OBEN'
    

def parse_brain():
    global i

    printer.signedinteger("my creature", data[i:i+4])
    i += 4
    
    # print(data[i:i+100])
    # 
    # assert data[i+12:i+12+8] == b'\x04\x00\x00\x00OBST' or data[i+12:i+12+8] == b'\x04\x00\x00\x00Lobe'
    
    num_lobes = struct.unpack('<I', data[i:i+4])[0]
    printer.integer("number of lobes", data[i:i+4])
    i += 4
    
    for _ in range(num_lobes):
        start_parse_object('Lobe')
        parse_lobe()
        end_parse_object()
        
    num_tracts = struct.unpack('<I', data[i:i+4])[0]
    printer.integer("number of tracts", data[i:i+4])
    i += 4
    
    for _ in range(num_tracts):
        start_parse_object('Tract')
        parse_tract()
        end_parse_object()
    
    num_lobes_plus_tracts = struct.unpack('<I', data[i:i+4])[0]
    printer.integer("number of lobes + tracts", data[i:i+4])
    i += 4
    assert num_lobes_plus_tracts == num_lobes + num_tracts

    printer.set_inline(True)
    for _ in range(num_lobes_plus_tracts):
        printer.integer("index of lobe/tract", data[i:i+4])
        i += 4
    printer.set_inline(False)
    
    printer.unknown(data[i:i+4])
    i += 4
            
    num_instincts = struct.unpack('<I', data[i:i+4])[0]
    printer.integer("number of instincts", data[i:i+4])
    i += 4

    for _ in range(num_instincts):
        start_parse_object('Instinct')
        parse_instinct()
        end_parse_object()
    
    printer.unknown(data[i:i+4])
    i += 4
            
    num_something = struct.unpack('<I', data[i:i+4])[0]
    printer.integer("number of something", data[i:i+4])
    i += 4
        
    for _ in range(num_something):
        printer.set_inline(True)
        printer.unknown(data[i:i+4])
        i += 4
        printer.unknown(data[i:i+4])
        i += 4
        printer.unknown_float(data[i:i+4])
        i += 4
        printer.set_inline(False)
    
    if CREATURESARCHIVE_DOCKINGSTATION:
        printer.unknown(data[i:i+4])
        i += 4
    
    assert data[i:i+8] == b'\x04\x00\x00\x00OBEN'

def parse_instinct():
    global i
    
    printer.integer("index of parent brain object", data[i:i+4])
    i += 4
    
    printer.set_inline(True)
    printer.integer("noun id", data[i:i+4])
    i += 4
    length = struct.unpack('<I', data[i:i+4])[0]
    s = data[i+4:i+4+length].decode('cp1252')
    i += 4 + length
    printer.string(s)
    printer.set_inline(False)
    
    printer.set_inline(True)
    printer.unknown(data[i:i+4])
    i += 4
    length = struct.unpack('<I', data[i:i+4])[0]
    s = data[i+4:i+4+length].decode('cp1252')
    i += 4 + length
    printer.string(s)
    printer.set_inline(False)
    
    printer.set_inline(True)
    printer.unknown(data[i:i+4])
    i += 4
    length = struct.unpack('<I', data[i:i+4])[0]
    s = data[i+4:i+4+length].decode('cp1252')
    i += 4 + length
    printer.string(s)
    printer.set_inline(False)
    
    printer.set_inline(True)
    printer.integer("action to be taken (motor)", data[i:i+4])
    i += 4
    printer.integer("drive id (reinforcer)", data[i:i+4])
    i += 4
    printer.float("drive change (reinforcement)", data[i:i+4])
    i += 4
    printer.integer("phase? to match C1/C2", data[i:i+4])
    i += 4
    printer.set_inline(False)
    
    assert data[i:i+8] == b'\x04\x00\x00\x00OBEN'
    
    # j = data[i:].find(b'\x04\x00\x00\x00OBEN')
    # printer.data(data[i:i+j])
    # i += j

def parse_tract():
    global i
    
    parse_braincomponent()
    
    name_length = struct.unpack("<I", data[i:i+4])[0]
    i += 4
    name = data[i:i+name_length].decode('cp1252')
    i += name_length
    printer.string(name)
    
    printer.set_inline(True)
    printer.integer("src lobe", data[i:i+4])
    i += 4
    printer.integer("src neuron_range_min", data[i:i+4])
    i += 4
    printer.integer("src neuron_range_max", data[i:i+4])
    i += 4
    printer.integer("src noOfDendritesPerNeuronOnEachPass", data[i:i+4])
    i += 4
    printer.integer("src neuralGrowthFactorStateVariableIndex", data[i:i+4])
    i += 4
    printer.set_inline(False)
    
    printer.set_inline(True)
    printer.integer("dest lobe", data[i:i+4])
    i += 4
    printer.integer("dest neuron_range_min", data[i:i+4])
    i += 4
    printer.integer("dest neuron_range_max", data[i:i+4])
    i += 4
    printer.integer("dest noOfDendritesPerNeuronOnEachPass", data[i:i+4])
    i += 4
    printer.integer("dest neuralGrowthFactorStateVariableIndex", data[i:i+4])
    i += 4
    printer.set_inline(False)
    
    num_dendrites = struct.unpack("<I", data[i:i+4])[0]
    printer.integer("num dendrites", data[i:i+4])
    i += 4
    
    j = data[i:].find(b'\x04\x00\x00\x00OBEN')
    printer.data(data[i:i+j])
    i += j
    assert data[i:i+8] == b'\x04\x00\x00\x00OBEN'



def parse_braincomponent():
    global i
    
    print("<div class=\"object superclass\">")
    print("<div class=\"object-name\">BrainComponent</div>")
    
    printer.set_inline(True)
    
    printer.integer("component id", data[i:i+4])
    i += 4
    
    printer.integer("update at time", data[i:i+4])
    i += 4
    
    printer.integer("SupportReinforcementFlag", data[i:i+4])
    i += 4
    printer.integer("RunInitRuleAlwaysFlag", data[i:i+4])
    i += 4
    printer.integer("Initialised", data[i:i+4])
    i += 4
    
    printer.set_inline(False)
    
    printer.named("init rule", data[i:i+256])
    i += 256
    
    printer.named("update rule", data[i:i+256])
    i += 256
    
    print("</div>")

def parse_lobe():
    global i
    
    parse_braincomponent()
    
    printer.set_inline(True)
    printer.integer("WinningNeuronId", data[i:i+4])
    i += 4
    printer.named("lobe token", data[i:i+4])
    i += 4
    printer.integer("TissueId", data[i:i+4])
    i += 4
    printer.set_inline(False)
    
    num_neurons = struct.unpack("<I", data[i:i+4])[0]
    printer.integer("number of neurons", data[i:i+4])
    i += 4
    
    for _ in range(num_neurons):
        print("<div class=\"object\">")
        print("<div class=\"object-name\">Neuron</div>")
        printer.integer("neuron id", data[i:i+4])
        i += 4
        printer.set_inline(True)
        for _ in range(8):
            printer.float("svrule variable", data[i:i+4])
            i += 4
        printer.set_inline(False)
        print("</div>")
    
    printer.set_inline(True)
    printer.integer("x", data[i:i+4])
    i += 4
    printer.integer("y", data[i:i+4])
    i += 4
    printer.integer("width", data[i:i+4])
    i += 4
    printer.integer("height", data[i:i+4])
    i += 4
    printer.set_inline(False)
    
    printer.set_inline(True)
    printer.integer("red", data[i:i+4])
    i += 4
    printer.integer("green", data[i:i+4])
    i += 4
    printer.integer("blue", data[i:i+4])
    i += 4
    printer.set_inline(False)
    
    printer.named("lobe name", data[i:i+5])
    i += 5
    
    printer.set_inline(True)
    for _ in range(num_neurons):
        printer.float("neuron input", data[i:i+4])
        i += 4
    printer.set_inline(True)
    
    # j = data[i:].find(b'\x04\x00\x00\x00OBEN')
    # printer.data(data[i:i+j])
    # i += j
    assert data[i:i+8] == b'\x04\x00\x00\x00OBEN'

def parse_bodypart():
    global i
    
    print("<div class=\"object superclass\">")
    print("<div class=\"object-name\">BodyPart</div>")
    
    print("<div class=\"object superclass\">")
    print("<div class=\"object-name\">Entity</div>")
    
    printer.set_inline(True)
    printer.integer("zorder", data[i:i+4])
    i += 4
    printer.integer("x", data[i:i+4])
    i += 4
    printer.integer("y", data[i:i+4])
    i += 4
    printer.integer("camera shy", data[i:i+4])
    i += 4
    printer.set_inline(False)
    
    printer.set_inline(True)
    printer.integer("sprite directory", data[i:i+4])
    i += 4
    
    length = struct.unpack('<I', data[i:i+4])[0]
    sprite = data[i+4:i+4+length].decode('cp1252')
    i += 4 + length
    printer.string(sprite)
    
    printer.integer("use local world directory", data[i:i+4])
    i += 4
    printer.set_inline(False)
    
    printer.set_inline(True)
    printer.integer("myAbsoluteBaseImage", data[i:i+4])
    i += 4
    printer.integer("myCurrentImageIndex", data[i:i+4])
    i += 4
    printer.integer("myCurrentBaseImageIndex", data[i:i+4])
    i += 4
    printer.integer("myNumberOfImages", data[i:i+4])
    i += 4
    printer.integer("myDefaultBaseImage", data[i:i+4])
    i += 4
    printer.integer("mySavedImageIndexState", data[i:i+4])
    i += 4
    printer.integer("myReadArchivedGalleryFlag", data[i:i+4])
    i += 4
    printer.set_inline(False)
    
    # note this might not be the right location
    if CREATURESARCHIVE_DOCKINGSTATION:
        printer.named("DS-only data?", data[i:i+8])
        i += 8
    
    printer.set_inline(True)
    printer.integer("overlay sprite directory", data[i:i+4])
    i += 4
    
    length = struct.unpack('<I', data[i:i+4])[0]
    overlay_sprite = data[i+4:i+4+length].decode('cp1252')
    i += 4 + length
    printer.string(overlay_sprite)
    
    printer.integer("use local world directory", data[i:i+4])
    i += 4
    printer.set_inline(False)
    
    printer.signedinteger("current overlay index", data[i:i+4])
    i += 4
    
    printer.named("animation (100*uint32)", data[i:i+400])
    i += 400
    
    # printer.unknown(data[i:i+4])
    # i += 4
    
    printer.set_inline(True)
    printer.integer("animation length?", data[i:i+4])
    i += 4
    
    printer.integer("animation next frame?", data[i:i+4])
    i += 4
    
    printer.integer("frame rate?", data[i:i+4])
    i += 4
    
    printer.integer("frame count?", data[i:i+4])
    i += 4
    
    printer.integer("width?", data[i:i+4])
    i += 4
    
    printer.integer("height?", data[i:i+4])
    i += 4
    
    printer.integer("visible?", data[i:i+4])
    i += 4
    printer.set_inline(False)
    print("</div>") # end of Entity
    
    printer.set_inline(True)
    printer.integer("angle?", data[i:i+4])
    i += 4
    
    printer.integer("view?", data[i:i+4])
    i += 4
    
    printer.integer("part?", data[i:i+4])
    i += 4
    
    printer.integer("genus?", data[i:i+4])
    i += 4
    
    printer.integer("sex?", data[i:i+4])
    i += 4
    
    printer.integer("variant?", data[i:i+4])
    i += 4
    
    printer.integer("last age?", data[i:i+4])
    i += 4
    printer.set_inline(False)
    
    printer.integer("overlay should override base sprite?", data[i:i+4])
    i += 4
    
    # printer.named("junk data?", data[i:i+12])
    # i += 12
    
    # printer.named("junk data?", data[i:i+31])
    # i += 31
    # 
    # printer.named("junk data?", data[i:i+5])
    # i += 5
    # 
    # printer.named("junk data?", data[i:i+36])
    # i += 36
    # 
    # printer.named("junk data?", data[i:i+134])
    # i += 134
    # 
    # printer.named("junk data?", data[i:i+202])
    # i += 202
    
    # note this might not be the right location
    if CREATURESARCHIVE_DOCKINGSTATION:
        printer.named("DS-only data", data[i:i+8])
        i += 8
    
    start_parse_object('BodyPartOverlay')
    parse_bodypartoverlay()
    end_parse_object()
    
    length = struct.unpack('<I', data[i:i+4])[0]
    s = data[i+4:i+4+length].decode('cp1252')
    i += 4 + length
    printer.string(s)
    
    printer.set_inline(True)
    printer.integer("body part id (0=head, 1=body, and so on)", data[i:i+4])
    i += 4
    
    printer.integer("mirrored", data[i:i+4])
    unknown_always_zero = struct.unpack('<I', data[i:i+4])[0]
    # assert unknown_always_zero == 0
    i += 4
    printer.set_inline(False)
    
    print("</div>")

def start_parse_object(type):
    global i
    global object_index
    j = i
    assert struct.unpack("<I", data[i:i+4])[0] == object_index + 1
    object_index += 1
    i += 4
    object_type = struct.unpack("<I", data[i:i+4])[0]
    i += 4
    if object_type in types:
        assert types[object_type] == type
    else:
        type_name_length = struct.unpack("<I", data[i:i+4])[0]
        i += 4
        type_name = data[i:i+type_name_length].decode('ascii')
        i += type_name_length
        assert type_name == type
        types[object_type] = type_name
    
    assert data[i:i+8] == b'\x04\x00\x00\x00OBST'
    
    print("<div class=\"object\">")
    print("<div class=\"object-name\">{} index={} type={}</div>".format(types[object_type], object_index, object_type))
    print("<div class=\"obst\">")
    print("{}".format(binrepr(data[j:i+8])))
    print("</div>")
    i += 8

def end_parse_object():
    global i
    assert(data[i:i+8] == b'\x04\x00\x00\x00OBEN')
    print("<div class=\"oben\">")
    print("{}".format(binrepr(data[i:i+8])))
    print("</div>")
    print("</div>") # end of object
    i += 8

def parse_body():
    global i
    
    # printer.note("Look, this is the same length as the start of CreatureHead! Probably some common superclass, like BodyPart")
    # 
    # printer.data(data[i:i+20])
    # i += 20
    # 
    # length = struct.unpack('<I', data[i:i+4])[0]
    # sprite = data[i+4:i+4+length].decode('cp1252')
    # i += 4 + length
    # printer.string(sprite)
    # 
    # printer.data(data[i:i+44])
    # i += 44
    # 
    # length = struct.unpack('<I', data[i:i+4])[0]
    # overlay_sprite = data[i+4:i+4+length].decode('cp1252')
    # i += 4 + length
    # printer.string(overlay_sprite)
    # 
    # printer.data(data[i:i+476])
    # i += 476
    # 
    # start_parse_object('BodyPartOverlay')
    # parse_bodypartoverlay()
    # end_parse_object()
    # 
    # printer.data(data[i:i+20])
    # i += 20
    
    parse_bodypart()
    
    for _ in range(4):
        printer.unknown(data[i:i+1])
        i += 1
    
    printer.named("body attachment data - 16 rows x 12 columns = 192 bytes", data[i:i+192])
    i += 192

def parse_creaturehead():
    global i
    
    parse_bodypart()
    
    printer.named("head attachment data - 16 rows x 4 columns = 64 bytes (Creatures Village has extra head data though...)", data[i:i+64])
    i += 64
    
    printer.signedinteger("next limb", data[i:i+4])
    i += 4
    
    for _ in range(3):
        start_parse_object("Limb")
        parse_limb()
        end_parse_object()
        
    printer.integer("current direction", data[i:i+1])
    i += 1
        
    printer.integer("hair state", data[i:i+4])
    i += 4
    
    printer.integer("head sprites count", data[i:i+4])
    i += 4
        
    # printer.data(data[i:i+9])
    # i += 9
    # 
    # printer.unknown(data[i:i+4])
    # i += 4
    
    assert data[i:i+8] == b'\x04\x00\x00\x00OBEN'

def parse_bodypartoverlay():
    global i
    
    printer.set_inline(True)
    printer.integer("parent index", data[i:i+4])
    i += 4
    
    overlay_length = struct.unpack("<I", data[i:i+4])[0]
    i += 4
    overlay_filename = data[i:i+overlay_length].decode('cp1252')
    printer.string(overlay_filename)
    i += overlay_length
    
    if CREATURESARCHIVE_DOCKINGSTATION:
        num_layers = 8
    else:
        num_layers = 4
    for _ in range(num_layers):
        printer.signedinteger("layer", data[i:i+4])
        i += 4
    printer.set_inline(False)
    
    assert data[i:i+8] == b'\x04\x00\x00\x00OBEN'

def parse_linguisticfaculty():
    global i
    printer.signedinteger("my creature", data[i:i+4])
    i += 4
    
    for category in ("verbs", "nouns", "adjectives", "other", "modifiers", "subjects", "drives"):
        num_words = struct.unpack('<I', data[i:i+4])[0]
        printer.integer("num " + category, data[i:i+4])
        i += 4
        
        for _ in range(num_words):
            printer.set_inline(True)
            length = struct.unpack('<I', data[i:i+4])[0]
            script = data[i+4:i+4+length].decode('cp1252') # first is an override? e.g. daphne - me
            printer.string(script)
            i += 4 + length
            
            length = struct.unpack('<I', data[i:i+4])[0]
            script = data[i+4:i+4+length].decode('cp1252')
            printer.string(script)
            i += 4 + length
            
            printer.float("strength", data[i:i+4])
            i += 4
            printer.integer("voice file initialised?", data[i:i+4])
            i += 4
            printer.set_inline(False)
    
    printer.signedinteger("stack count?", data[i:i+4])
    i += 4
    
    assert data[i:i+8] == b'\x04\x00\x00\x00OBEN'

def parse_lifefaculty():
    global i
    printer.signedinteger("my creature", data[i:i+4])
    i += 4
    
    j = data[i:].find(b'\x04\x00\x00\x00OBEN')
    printer.data(data[i:i+j])
    i += j
    assert data[i:i+8] == b'\x04\x00\x00\x00OBEN'

def parse_expressivefaculty():
    global i
    printer.signedinteger("my creature", data[i:i+4])
    i += 4
    
    j = data[i:].find(b'\x04\x00\x00\x00OBEN')
    printer.data(data[i:i+j])
    i += j
    assert data[i:i+8] == b'\x04\x00\x00\x00OBEN'

def parse_musicfaculty():
    global i
    assert data[i+4:i+12] == b'\x04\x00\x00\x00OBEN'
    printer.signedinteger("my creature", data[i:i+4])
    i += 4
    
    j = data[i:].find(b'\x04\x00\x00\x00OBEN')
    printer.data(data[i:i+j])
    i += j
    assert data[i:i+8] == b'\x04\x00\x00\x00OBEN'

def parse_receptor():
    global i
    
    printer.set_inline(True)
    printer.integer("organ?", data[i:i+4])
    i += 4
    printer.integer("tissue?", data[i:i+4])
    i += 4
    printer.integer("locus?", data[i:i+4])
    i += 4
    printer.integer("chemical?", data[i:i+4])
    i += 4
    printer.set_inline(False)
    printer.set_inline(True)
    printer.float("threshold?", data[i:i+4])
    i += 4
    printer.float("nominal?", data[i:i+4])
    i += 4
    printer.float("gain?", data[i:i+4])
    i += 4
    printer.set_inline(False)
    printer.set_inline(True)
    for _ in range(3):
        printer.unknown(data[i:i+4])
        i += 4
    printer.set_inline(False)
    
    assert data[i:i+8] == b'\x04\x00\x00\x00OBEN'

def parse_macroscript():
    global i
    
    printer.set_inline(True)
    printer.unknown(data[i:i+4])
    i += 4
    printer.integer("family", data[i:i+4])
    i += 4
    printer.integer("genus", data[i:i+4])
    i += 4
    printer.integer("species", data[i:i+4])
    i += 4
    printer.integer("event number", data[i:i+4])
    i += 4
    printer.unknown(data[i:i+4])
    i += 4
    printer.set_inline(False)
    
    printer.set_inline(True)
    remaining_bytes = struct.unpack('<I', data[i:i+4])[0]
    printer.integer("num remaining bytes", data[i:i+4])
    i += 4
    printer.set_inline(False)
    
    printer.named("bytecode", data[i:i+remaining_bytes])
    i += remaining_bytes
    
    start_parse_object('DebugInfo')
    parse_debuginfo()
    end_parse_object()
    
    assert data[i:i+8] == b'\x04\x00\x00\x00OBEN'
    
    # 
    # printer.set_inline(True)
    # printer.integer("command id (SETV)", data[i:i+4])
    # i += 4
    # printer.unknown(data[i:i+2])
    # i += 2
    # printer.unknown(data[i:i+2])
    # i += 2
    # printer.unknown(data[i:i+4])
    # i += 4
    # printer.unknown(data[i:i+4])
    # i += 4
    # printer.unknown(data[i:i+4])
    # i += 4
    # printer.unknown(data[i:i+2])
    # i += 2
    # printer.unknown(data[i:i+4])
    # i += 4
    # printer.unknown(data[i:i+4])
    # i += 4
    # printer.unknown(data[i:i+4])
    # i += 4
    # printer.unknown(data[i:i+4])
    # i += 4
    # printer.unknown(data[i:i+2])
    # i += 2
    # printer.set_inline(False)
    # 
    # printer.set_inline(True)
    # printer.integer("command id (POSE)", data[i:i+4])
    # i += 4
    # 
    # printer.integer("argument", data[i:i+4])
    # i += 4
    # printer.set_inline(False)
    # 
    # printer.set_inline(True)
    # printer.integer("command id (SETV)", data[i:i+4])
    # i += 4
    # 
    # printer.unknown(data[i:i+2])
    # i += 2
    # 
    # printer.unknown(data[i:i+2])
    # i += 2
    # 
    # printer.unknown(data[i:i+2])
    # i += 2
    # 
    # printer.unknown(data[i:i+2])
    # i += 2
    # 
    # printer.unknown(data[i:i+4])
    # i += 4
    # 
    # printer.unknown(data[i:i+4])
    # i += 4
    # 
    # printer.set_inline(False)
    # printer.set_inline(True)
    # 
    # printer.integer("command id (WAIT)", data[i:i+2])
    # i += 2
    # 
    # printer.unknown(data[i:i+2])
    # i += 2
    # 
    # printer.unknown(data[i:i+2])
    # i += 2
    # 
    # printer.integer("command id (GOTO)", data[i:i+2])
    # i += 2
    # 
    # printer.integer("byte offset", data[i:i+4])
    # i += 4
    # 
    # 
    # 
    # 
    # printer.set_inline(False)
    # printer.set_inline(True)
    # 
    # printer.integer("command id (ELSE)", data[i:i+2])
    # i += 2
    # 
    # printer.integer("byte offset?", data[i:i+4])
    # i += 4
    # 
    # printer.set_inline(False)
    # 
    # printer.set_inline(True)
    # printer.integer("command id (POSE)", data[i:i+2])
    # i += 2
    # printer.unknown(data[i:i+2])
    # i += 2
    # printer.integer("argument", data[i:i+4])
    # i += 4
    # printer.set_inline(False)
    # 
    # printer.set_inline(True)
    # printer.integer("command id (SETV)", data[i:i+2])
    # i += 2
    # printer.unknown(data[i:i+2])
    # i += 2
    # printer.unknown(data[i:i+2])
    # i += 2
    # printer.unknown(data[i:i+2])
    # i += 2
    # printer.unknown(data[i:i+2])
    # i += 2
    # printer.unknown(data[i:i+2])
    # i += 2
    # printer.unknown(data[i:i+2])
    # i += 2
    # printer.unknown(data[i:i+2])
    # i += 2
    # printer.unknown(data[i:i+2])
    # i += 2
    # printer.unknown(data[i:i+2])
    # i += 2
    # printer.set_inline(False)
    # 
    # printer.set_inline(True)
    # printer.integer("command id (WAIT)", data[i:i+2])
    # i += 2
    # printer.unknown(data[i:i+2])
    # i += 2
    # printer.unknown(data[i:i+2])
    # i += 2
    # printer.set_inline(False)
    # 
    # printer.set_inline(True)
    # printer.integer("command id (POSE)", data[i:i+2])
    # i += 2
    # printer.unknown(data[i:i+2])
    # i += 2
    # printer.integer("argument", data[i:i+4])
    # i += 4
    # printer.set_inline(False)
    # 
    # printer.set_inline(True)
    # printer.integer("command id (SETV)", data[i:i+2])
    # i += 2
    # 
    # printer.unknown(data[i:i+2])
    # i += 2
    # printer.unknown(data[i:i+2])
    # i += 2
    # printer.integer("command id (RAND)", data[i:i+2])
    # i += 2
    # printer.integer("argument type", data[i:i+2])
    # i += 2
    # printer.integer("argument", data[i:i+4])
    # i += 4
    # printer.integer("argument type", data[i:i+2])
    # i += 2
    # printer.integer("argument", data[i:i+4])
    # i += 4
    # printer.set_inline(False)
    # 
    # printer.set_inline(True)
    # printer.integer("command id (WAIT)", data[i:i+2])
    # i += 2
    # printer.integer("argument type (variable)", data[i:i+2])
    # i += 2
    # printer.integer("command id (VA00)", data[i:i+2])
    # i += 2
    # printer.set_inline(False)
    # 
    # 
    # printer.set_inline(True)
    # printer.integer("command id (ENDI)", data[i:i+2])
    # i += 2
    # printer.set_inline(False)
    # 
    # printer.set_inline(True)
    # printer.integer("command id (STIM)", data[i:i+2])
    # i += 2
    # printer.integer("command id (WRIT)", data[i:i+2])
    # i += 2
    # printer.integer("argument type (agent)", data[i:i+2])
    # i += 2
    # printer.integer("command id (TARG)", data[i:i+2])
    # i += 2
    # printer.integer("argument type", data[i:i+2])
    # i += 2
    # printer.integer("argument", data[i:i+4])
    # i += 4
    # printer.integer("argument type", data[i:i+2])
    # i += 2
    # printer.integer("argument", data[i:i+4])
    # i += 4
    # printer.unknown(data[i:i+2])
    # i += 2
    # printer.set_inline(False)
    
def parse_biochemistry():
    global i
    
    printer.signedinteger("my creature", data[i:i+4])
    i += 4
    
    for _ in range(256):
        print("<div class=\"object\">")
        print("<div class=\"object-name\">ChemicalData</div>")
        printer.set_inline(True)
        printer.float("decay", data[i:i+4])
        i += 4
        printer.float("concentration", data[i:i+4])
        i += 4
        printer.set_inline(False)
        print("</div>")
    
    for _ in range(128):
        print("<div class=\"object\">")
        print("<div class=\"object-name\">Neuro emitter</div>")
        printer.set_inline(True)
        printer.integer("bio tick rate", data[i:i+4])
        i += 4
        printer.integer("bio tick", data[i:i+4])
        i += 4
        printer.set_inline(False)
        
        printer.set_inline(True)
        for _ in range(3):
            printer.signedinteger("neuronal input floatref", data[i:i+4])
            i += 4
        printer.set_inline(False)
        
        
        for _ in range(4):
            printer.set_inline(True)
            printer.integer("chemical id", data[i:i+4])
            i += 4
            printer.integer("chemical amount", data[i:i+4])
            i += 4
            printer.set_inline(False)
        print("</div>")
    
    printer.integer("number neuro emitters", data[i:i+4])
    i += 4
    
    num_organs = struct.unpack("<I", data[i:i+4])[0]
    printer.integer("number of organs", data[i:i+4])
    i += 4
    
    for _ in range(num_organs):
        start_parse_object('Organ')
        parse_organ()
        end_parse_object()
        
    assert data[i:i+8] == b'\x04\x00\x00\x00OBEN'
    

# u32 event_number
# u32 unknown - 0 - EMIT's command group? or something random?
# u32 unknown - 14 - ???
# u32 unknown - 107 - EMIT's command index?
# u32 unknown - 1 - first argument, a raw integer. how does it know?
# u16 unknown - 3 - ? third caos command group? (_P1_ is in group 2, which is the third one...)
# u16 unknown - 303 - _P1_'s command index!
# u16 unknown - 17 - ???
# u32 object index? - 3

def parse_reproductivefaculty():
    global i
    
    printer.signedinteger("my creature", data[i:i+4])
    i += 4
    
    printer.set_inline(True)
    for _ in range(3):
        printer.unknown(data[i:i+4])
        i += 4
    printer.unknown_float(data[i:i+4])
    i += 4
    for _ in range(3):
        printer.unknown(data[i:i+4])
        i += 4
    printer.set_inline(False)
    
    assert data[i:i+8] == b'\x04\x00\x00\x00OBEN'

def parse_motorfaculty():
    global i
    
    printer.signedinteger("my creature", data[i:i+4])
    i += 4

    # for (int i=0; i<NUMINVOL; i++) {
    # 	archive >> myInvoluntaryActions[i].latency;
    # 	archive.ReadFloatRefTarget( myInvoluntaryActions[i].locus );
    # }
    
    printer.set_inline(True)
    printer.signedinteger("myCurrentFocusOfAttention", data[i:i+4])
    i += 4
    printer.signedinteger("myCurrentAction", data[i:i+4])
    i += 4
    printer.signedinteger("myCurrentInvoluntaryAction", data[i:i+4])
    i += 4

    printer.signedinteger("attentionScriptNo", data[i:i+4])
    i += 4
    printer.signedinteger("decisionScriptNo", data[i:i+4])
    i += 4
    printer.set_inline(False)
    
    for _ in range(5):
        printer.set_inline(True)
        printer.signedinteger("involuntary action latency", data[i:i+4])
        i += 4
        printer.float("involuntary action locus", data[i:i+4])
        i += 4
        printer.set_inline(False)
    
    if CREATURESARCHIVE_DOCKINGSTATION:
        printer.named("DS-only data?", data[i:i+4])
        i += 4
    
    assert data[i:i+8] == b'\x04\x00\x00\x00OBEN'

def parse_organ():
    global i
    
    
    printer.set_inline(True)
    for _ in range(11):
        printer.unknown_float(data[i:i+4])
        i += 4
    printer.unknown(data[i:i+4])
    i += 4
    printer.set_inline(False)
    
    printer.set_inline(True)
    num_receptors = struct.unpack("<I", data[i:i+4])[0]
    printer.integer("number of receptors", data[i:i+4])
    i += 4
    num_receptor_groups = struct.unpack("<I", data[i:i+4])[0]
    printer.integer("number of receptor groups", data[i:i+4])
    i += 4
    num_reactions = struct.unpack("<I", data[i:i+4])[0]
    printer.integer("number of reactions", data[i:i+4])
    i += 4
    printer.set_inline(False)
    
    for _ in range(num_reactions):
        print("<div class=\"object\">")
        print("<div class=\"object-name\">Reaction</div>")
        
        printer.set_inline(True)
        printer.float("r1_amount", data[i:i+4])
        i += 4
        printer.integer("r1_chemical", data[i:i+4])
        i += 4
        printer.float("r2_amount", data[i:i+4])
        i += 4
        printer.integer("r2_chemical", data[i:i+4])
        i += 4
        printer.set_inline(False)
        
        printer.set_inline(True)
        printer.float("rate", data[i:i+4])
        i += 4
        printer.set_inline(False)
        
        printer.set_inline(True)
        printer.float("p1_amount", data[i:i+4])
        i += 4
        printer.integer("p1_chemical", data[i:i+4])
        i += 4
        printer.float("p2_amount", data[i:i+4])
        i += 4
        printer.integer("p2_chemical", data[i:i+4])
        i += 4
        printer.set_inline(False)
        
        print("</div>")
    
    for _ in range(num_receptor_groups):
        receptors_in_group = struct.unpack("<I", data[i:i+4])[0]
        printer.integer("receptors_in_group", data[i:i+4])
        i += 4
        
        for _ in range(receptors_in_group):
            start_parse_object('Receptor')
            parse_receptor()
            end_parse_object()
    
    num_emitters = struct.unpack("<I", data[i:i+4])[0]
    printer.integer("number of emitters", data[i:i+4])
    i += 4
    for _ in range(num_emitters):
        print("<div class=\"object\">")
        print("<div class=\"object-name\">Emitter</div>")
        
        printer.set_inline(True)
        printer.integer("organ?", data[i:i+4])
        i += 4
        printer.integer("tissue?", data[i:i+4])
        i += 4
        printer.integer("locus?", data[i:i+4])
        i += 4
        printer.integer("chemical?", data[i:i+4])
        i += 4
        printer.set_inline(False)
        
        printer.set_inline(True)
        printer.float("threshold?", data[i:i+4])
        i += 4
        printer.float("rate?", data[i:i+4])
        i += 4
        printer.float("gain?", data[i:i+4])
        i += 4
        printer.set_inline(False)
        
        printer.set_inline(True)
        printer.unknown(data[i:i+4])
        i += 4
        printer.unknown_float(data[i:i+4])
        i += 4
        printer.set_inline(False)
        
        print("</div>")
    
    
    printer.integer("index of parent Biochemistry object", data[i:i+4])
    i += 4
    
    assert data[i:i+8] == b"\x04\x00\x00\x00OBEN"

def parse_sensoryfaculty():
    global i
    
    printer.signedinteger("my creature", data[i:i+4])
    i += 4
    
    j = data[i:].find(b'\x04\x00\x00\x00OBEN')
    printer.data(data[i:i+j])
    i += j

def parse_creature():
    global i
    
    printer.data(data[i:i+296])
    i += 296
    
    # print(data[i:i+100])
    
    if data[i:i+4] == b'\xfe\xff\xff\xff':
        printer.signedinteger("MacroScript", data[i:i+4])
        i += 4
    else:
        start_parse_object("MacroScript")
        parse_macroscript()
        end_parse_object()

    
    
    number_of_something = struct.unpack("<I", data[i:i+4])[0]
    printer.integer("number of something", data[i:i+4])
    i += 4
    
    for _ in range(number_of_something):
        printer.integer("number of something", data[i:i+4])
        i += 4
    
    printer.unknown(data[i:i+4])
    i += 4
    
    num_vars = struct.unpack("<I", data[i:i+4])[0]
    printer.integer("number vaXX", data[i:i+4])
    i += 4
    assert num_vars == 100
    
    for _ in range(num_vars):
        printer.set_inline(True)
        var_type = struct.unpack("<I", data[i:i+4])[0]
        printer.integer("type? 0=int 1=float 2=string", data[i:i+4])
        i += 4
        
        if var_type == 0:
            printer.integer("value", data[i:i+4])
            i += 4
        elif var_type == 1:
            printer.float("value", data[i:i+4])
            i += 4
        elif var_type == 2:
            length = struct.unpack('<I', data[i:i+4])[0]
            s = data[i+4:i+4+length].decode('cp1252')
            i += 4 + length
            printer.string(s)
        elif var_type == 4:
            printer.integer("4 value?", data[i:i+4])
            i += 4
        else:
            raise NotImplementedError(var_type)
            
        printer.unknown(data[i:i+4])
        i += 4
            
        printer.set_inline(False)
    
    length = struct.unpack('<I', data[i:i+4])[0]
    s = data[i+4:i+4+length].decode('cp1252')
    printer.string(s)
    i += 4 + length
    
    printer.data(data[i:i+1212])
    i += 1212
    
    num_monikers = struct.unpack('<I', data[i:i+4])[0]
    printer.integer("num monikers", data[i:i+4])
    i += 4
    
    for _ in range(num_monikers):
        length = struct.unpack('<I', data[i:i+4])[0]
        moniker = data[i+4:i+4+length].decode('cp1252')
        printer.string(moniker)
        i += 4 + length
    
    printer.data(data[i:i+624])
    i += 624
    
    printer.integer("double speed", data[i:i+4])
    i += 4
    
    printer.integer("holding hands with pointer", data[i:i+4])
    i += 4
    
    printer.note("mother moniker")
    length = struct.unpack('<I', data[i:i+4])[0]
    moniker = data[i+4:i+4+length].decode('cp1252')
    printer.string(moniker)
    i += 4 + length
    
    printer.note("father moniker")
    length = struct.unpack('<I', data[i:i+4])[0]
    moniker = data[i+4:i+4+length].decode('cp1252')
    printer.string(moniker)
    i += 4 + length
    
    # 17 body parts
    printer.note("17 body parts")
    for _ in range(17):
        printer.set_inline(True)
        printer.integer("body part variant", data[i:i+4])
        i += 4
        printer.integer("body part genera?", data[i:i+4])
        i += 4
        printer.integer("body part sprite?", data[i:i+4])
        i += 4
        printer.set_inline(False)
    
    # printer.data(data[i:i+204])
    # i += 204
    
    start_parse_object("Body")
    # exit()
    parse_body()
    end_parse_object()
    
    start_parse_object("CreatureHead")
    parse_creaturehead()
    end_parse_object()

    for _ in range(5):
        start_parse_object("Limb")
        parse_limb()
        end_parse_object()
    
    printer.data(data[i:i+6144])
    i += 6144
    
    start_parse_object("SensoryFaculty")
    parse_sensoryfaculty()
    end_parse_object()
    
    start_parse_object("Brain")
    parse_brain()
    end_parse_object()
    
    start_parse_object("MotorFaculty")
    parse_motorfaculty()
    end_parse_object()
    
    start_parse_object("LinguisticFaculty")
    parse_linguisticfaculty()
    end_parse_object()
    
    start_parse_object("ReproductiveFaculty")
    parse_reproductivefaculty()
    end_parse_object()
    
    start_parse_object("ExpressiveFaculty")
    parse_expressivefaculty()
    end_parse_object()
    
    start_parse_object("MusicFaculty")
    parse_musicfaculty()
    end_parse_object()
    
    start_parse_object("LifeFaculty")
    parse_lifefaculty()
    end_parse_object()
    
    start_parse_object("Biochemistry")
    parse_biochemistry()
    end_parse_object()
    
    assert data[i:i+8] == b"\x04\x00\x00\x00OBEN"

def parse_skeletalcreature():
    global i
    
    printer.data(data[i:i+232])
    i += 232
    
    num_caos_machines = struct.unpack('<I', data[i:i+4])[0]
    printer.integer("num caos machines?", data[i:i+4])
    i += 4
    
    for _ in range(num_caos_machines):
        start_parse_object("CAOSMachine")
        parse_caosmachine()
        end_parse_object()
    
    num_caos_machines = struct.unpack('<I', data[i:i+4])[0]
    printer.integer("num caos machines?", data[i:i+4])
    i += 4
    
    for _ in range(num_caos_machines):
        start_parse_object("CAOSMachine")
        parse_caosmachine()
        end_parse_object()
    
    # # could be a second CAOSMachine??
    # if data[i:i+4] == b'\xfe\xff\xff\xff':
    #     pass
    # else:
    #     start_parse_object("CAOSMachine")
    #     parse_caosmachine()
    #     end_parse_object()
    
    printer.data(data[i:i+1212])
    i += 1212
    
    number_pairs = struct.unpack("<I", data[i:i+4])[0]
    printer.integer("number of pairs?", data[i:i+4])
    i += 4
    
    for _ in range(number_pairs):
        printer.set_inline(True)
        
        type = struct.unpack("<I", data[i:i+4])[0]
        printer.integer("type? (0=int 2=string)", data[i:i+4])
        i += 4
        
        if type == 0:
            printer.integer("value", data[i:i+4])
            i += 4
        elif type == 2:
            length = struct.unpack('<I', data[i:i+4])[0]
            s = data[i+4:i+4+length].decode('cp1252')
            printer.string(s)
            i += 4 + length
        else:
            raise NotImplementedError(type)
        
        printer.unknown(data[i:i+4])
        i += 4
        
        type = struct.unpack("<I", data[i:i+4])[0]
        printer.integer("type? (0=int 2=string)", data[i:i+4])
        i += 4
        
        if type == 0:
            printer.integer("value", data[i:i+4])
            i += 4
        elif type == 2:
            length = struct.unpack('<I', data[i:i+4])[0]
            s = data[i+4:i+4+length].decode('cp1252')
            printer.string(s)
            i += 4 + length
        else:
            raise NotImplementedError(type)
        
        printer.data(data[i:i+4])
        i += 4
        printer.set_inline(False)
    
    
    num_monikers = struct.unpack("<I", data[i:i+4])[0]
    printer.integer("number monikers?", data[i:i+4])
    i += 4
    
    for _ in range(num_monikers):
        length = struct.unpack('<I', data[i:i+4])[0]
        moniker = data[i+4:i+4+length].decode('cp1252')
        printer.string(moniker)
        i += 4 + length
    
    printer.unknown(data[i:i+4])
    i += 4
    
    printer.data(data[i:i+584])
    i += 584
    
    # if "Brendan" and True:
    #     printer.unknown(data[i:i+4])
    #     i += 4
    
    printer.unknown_float(data[i:i+4])
    i += 4
    
    printer.unknown_float(data[i:i+4])
    i += 4
    
    printer.data(data[i:i+160])
    i += 160
    
    length = struct.unpack('<I', data[i:i+4])[0]
    moniker = data[i+4:i+4+length].decode('cp1252')
    printer.string(moniker)
    i += 4 + length
    
    length = struct.unpack('<I', data[i:i+4])[0]
    moniker = data[i+4:i+4+length].decode('cp1252')
    printer.string(moniker)
    i += 4 + length
    
    for _ in range(17):
        printer.set_inline(True)
        
        printer.unknown(data[i:i+4])
        i += 4
        
        printer.unknown(data[i:i+4])
        i += 4
        
        printer.integer("some incrementing value", data[i:i+4])
        i += 4
        
        printer.set_inline(False)
    
    # printer.data(data[i:i+204])
    # i += 204
    
    # length = struct.unpack('<I', data[i:i+4])[0]
    # moniker = data[i+4:i+4+length]
    # printer.string(repr(moniker))
    # i += 4 + length
    # 
    # length = struct.unpack('<I', data[i:i+4])[0]
    # moniker = data[i+4:i+4+length]
    # printer.string(repr(moniker))
    # i += 4 + length
    # 
    # printer.data(data[i:i+188])
    # i += 188
    
    start_parse_object("Body")
    # exit()
    parse_body()
    end_parse_object()
    
    start_parse_object("CreatureHead")
    parse_creaturehead()
    end_parse_object()

    for _ in range(5):
        start_parse_object("Limb")
        parse_limb()
        end_parse_object()
    
    printer.data(data[i:i+6144])
    i += 6144
    
    start_parse_object("SensoryFaculty")
    parse_sensoryfaculty()
    end_parse_object()
    
    start_parse_object("Brain")
    parse_brain()
    end_parse_object()
    
    start_parse_object("MotorFaculty")
    parse_motorfaculty()
    end_parse_object()
    
    start_parse_object("LinguisticFaculty")
    parse_linguisticfaculty()
    end_parse_object()
    
    start_parse_object("ReproductiveFaculty")
    parse_reproductivefaculty()
    end_parse_object()
    
    start_parse_object("ExpressiveFaculty")
    parse_expressivefaculty()
    end_parse_object()
    
    start_parse_object("MusicFaculty")
    parse_musicfaculty()
    end_parse_object()
    
    start_parse_object("LifeFaculty")
    parse_lifefaculty()
    end_parse_object()
    
    start_parse_object("Biochemistry")
    parse_biochemistry()
    end_parse_object()
    
    assert data[i:i+8] == b"\x04\x00\x00\x00OBEN"

def parse_debuginfo():
    global i
    length = struct.unpack('<I', data[i:i+4])[0]
    script = data[i+4:i+4+length].decode('cp1252')
    printer.string(script)
    i += 4 + length
    
    printer.set_inline(True)
    num_commands = struct.unpack('<I', data[i:i+4])[0]
    printer.integer("num commands", data[i:i+4])
    i += 4
    printer.set_inline(False)
    
    for _ in range(num_commands):
        printer.set_inline(True)
        printer.integer("macroscript byte offset", data[i:i+4])
        i+=4
        printer.integer("debuginfo string offset".format(_+1), data[i:i+4])
        i+=4
        printer.set_inline(False)
    
    # if data[i+116:i+116+8] == b'\x04x00x00x00OBEN':
    # for _ in range(29):
    #     print("<div style=\"margin-left: {}px\">{}</div>".format(depth * 10, struct.unpack('<I', data[i:i+4])[0]))
    #     i += 4

serialization_version = struct.unpack("<I", data[i:i+4])[0]
printer.integer("serialization version (12=c3 39=ds)", data[i:i+4])
i += 4

while i < len(data):
    if i + 4 < len(data) and data[i:i+4] == b'OBST':
        assert(data[i-4:i] == b'\x04\x00\x00\x00')
        j = i-5
        if data[j] != 0:
            while data[j] != 0:
                j -= 1
            j -= 3
            name_length = struct.unpack('<I', data[j:j+4])[0]
            # print("name length", name_length)
            name = data[j+4:j+4+name_length]
            j-=4
            object_type = struct.unpack('<I', data[j:j+4])[0]
            types[object_type] = name.decode('ascii')
        else:
            j -= 3
            object_type = struct.unpack('<I', data[j:j+4])[0]
        
        j -= 4
        object_index = struct.unpack('<I', data[j:j+4])[0]
        
        
        # j-4 to also include object index
        current_span = current_span[:len(current_span) - (i - j)]
        printer.data(current_span)
        current_span = b''
        
        print("<div class=\"object\">")
        print("<div class=\"object-name\">{} index={} type={}</div>".format(types[object_type], object_index, object_type))
        print("<div class=\"obst\">")
        print("{}".format(binrepr(data[j:i+4])))
        print("</div>")
        # spans.append((j-4, i+4, 'OBST', types[object_type]))
        # print("    " * depth + "{} at {}".format(types[object_type], i))
        i += 4
        
        if types[object_type] == 'DebugInfo':
            parse_debuginfo()
        if types[object_type] == 'MacroScript':
            parse_macroscript()
        if types[object_type] == 'Body':
            parse_body()
        if types[object_type] == 'CreatureHead':
            parse_creaturehead()
        if types[object_type] == 'BodyPartOverlay':
            parse_bodypartoverlay()
        if types[object_type] == 'Limb':
            parse_limb()
        if types[object_type] == 'Brain':
            parse_brain()
        if types[object_type] == 'Lobe':
            parse_lobe()
        if types[object_type] == 'SkeletalCreature':
            parse_skeletalcreature()
        if types[object_type] == 'Creature':
            parse_creature()
        if types[object_type] == 'MusicFaculty':
            parse_musicfaculty()
        if types[object_type] == 'CAOSMachine':
            parse_caosmachine()
        if types[object_type] == 'MotorFaculty':
            parse_motorfaculty()
        if types[object_type] == 'ReproductiveFaculty':
            parse_reproductivefaculty()
        if types[object_type] == 'LinguisticFaculty':
            parse_linguisticfaculty()
        if types[object_type] == 'Receptor':
            parse_receptor()
        if types[object_type] == 'Biochemistry':
            parse_biochemistry()
        if types[object_type] == 'Organ':
            parse_organ()
        
    elif i + 4 < len(data) and data[i:i+4] == b'OBEN':
        assert(data[i-4:i] == b'\x04\x00\x00\x00')
        current_span = current_span[:len(current_span) - 4]
        printer.data(current_span)
        current_span = b''
        print("<div class=\"oben\">")
        print("{}".format(binrepr(data[i-4:i+4])))
        print("</div>")
        print("</div>") # end of .object
        # print("    " * depth + "end of object")
        i += 4
        # spans.append((i-4, i+4, 'OBEN'))
    else:
        current_span += data[i:i+1]
        i += 1

# assert(depth == 1)
printer.data(current_span)

print("</div>")
        
        
# 
# print(types)
# print(spans)
# 
# new_spans = []
# p = 0
# for _ in spans:
#     if _[0] > p:
#         new_spans.append((p, _[0], 'data', data[p:_[0]]))
#     new_spans.append(_)
# 
# # print(new_spans)
# 
# 
# class buffer:
#     def __init__(self, buf_):
#         self._buf = buf_
#     def __repr__(self):
#         return repr(self._buf)
# 
# class genericobject:
#     def __init__(self, name):
#         self.name = name
#         self.data = []
# 
# class parser:
#     def __init__(self, new_spans_):
#         self.p = 0
#         self.new_spans = new_spans_
# 
#     def parse_object(self):
#         print("parse_object")
#         ret = []
#         self.new_spans[self.p][2] == 'OBST'
#         ret.append(self.new_spans[self.p][3])
#         self.p += 1
# 
#         while self.p < len(self.new_spans):
#             assert(self.new_spans[self.p][2] in ('OBST', 'OBEN', 'data'))
#             if self.new_spans[self.p][2] == 'OBST':
#                 ret.append(self.parse_object())
#             elif self.new_spans[self.p][2] == 'OBEN':
#                 self.p += 1
#                 return ret
#             else:
#                 ret.append(buffer(self.new_spans[self.p][3]))
#                 self.p += 1
#         print(ret)
#         assert(False)
# 
#     def parse_toplevel(self):
#         ret = []
#         while self.p < len(self.new_spans):
#             assert(self.new_spans[self.p][2] in ('OBST', 'data'))
#             if self.new_spans[self.p][2] == 'OBST':
#                 ret.append(self.parse_object())
#             else:
#                 ret.append(buffer(self.new_spans[self.p][3]))
#                 self.p += 1
#         return ret
# 
# p = parser(new_spans)    
# print(p.parse_toplevel())

        # print(data[j:i])


exit()





# for world saves



with open("SpaceAndAllThatIsOutThere.out", "rb") as f:
    data = f.read()

cur = cursor(data)

creaturesarchivemagic = cur.read_s32le()
print("creaturesarchive magic", creaturesarchivemagic)
assert(creaturesarchivemagic == 0x27)

creaturesarchivetype = cur.read_s32le()
print("creaturesarchive type", creaturesarchivetype)
assert(creaturesarchivetype == 0x0)

cur.read_unknown_s32le(expected=0)

number = cur.read_s32le()
print("number", number)

for _ in range(number):
    # cur.read_unknown_s32le(expected=1)
    bytes = cur.read(9)
    print("bytes", list(bytes))

cur.read_unknown_s32le(expected=0)
number_of_objects = cur.read_s32le()
print("number of objects", number_of_objects)

cur.read_unknown_s32le(expected=0) # object index
cur.read_unknown_s32le(expected=0) # object type
length = cur.read_s32le()
name = cur.read(length)
print("name", name)

def parse_generic_object():
    # cur.read_unknown_s32le(expected=4) # length of OBST?
    obst = cur.read(8)
    if obst != b'\x04\x00\x00\x00OBST':
        raise Exception("Expected \x04\x00\x00\x00OBST, got {}".format(obst))
    depth = 1
    while True:
        if cur.peek(4) == b'OBST':
            depth += 1
            cur.read(4)
            continue
        elif cur.peek(4) == b'OBEN':
            depth -= 1
            cur.read(4)
            if depth == 0:
                break
            else:
                continue
        else:
            cur.read(1)
    return '<object>'

print(parse_generic_object())
for _ in range(number_of_objects - 1):
    object_index = cur.read_s32le()
    object_depth = cur.read_s32le()
    # print("object index", cur.read_s32le()) 
    # print("object depth", cur.read_s32le())
    parse_generic_object()

# number_of_objects = cur.read_s32le()
# print("number of objects", number_of_objects)

cur.read_unknown_s32le(0x4c5) # number of objects?
cur.read_unknown_s32le(0xa)
cur.read_unknown_s32le(0x718)
cur.read_unknown_s32le(0x12)
cur.read_unknown_s32le(0x12)
print("unknown", cur.read(20))
cur.read_unknown_s32le(0xfa)
cur.read_unknown_s32le(0xfa)
cur.read_unknown_s32le(0xc)
# cur.read_unknown_s32le(0x0)

object_index = cur.read_s32le()
print("object index", object_index)
object_type = cur.read_s32le()
print("object depth", object_type) # Room = 0x2
length = cur.read_s32le()
name = cur.read(length) # Room
print("name", name)

parse_generic_object()
for _ in range(3):
    object_index = cur.read_s32le()
    object_type = cur.read_s32le()
    parse_generic_object()

junk = cur.read(2384) # junk???
for _ in range(596):
    assert junk[_ * 4 : (_ + 1) * 4] == b'\xfe\xff\xff\xff'

for _ in range(1217):
    object_index = cur.read_s32le()
    object_type = cur.read_s32le()
    parse_generic_object()
    
junk = cur.read(4888)
print("junk", junk)

object_index = cur.read_s32le()
print("object index", object_index)
object_type = cur.read_s32le()
print("object depth", object_type) # MetaRoom = 0x5
length = cur.read_s32le()
name = cur.read(length) # MetaRoom
print("name", name)

parse_generic_object()

junk = cur.read(36) # all -2s

for _ in range(1, 10):
    print("metaroom", _)
    object_index = cur.read_s32le()
    object_type = cur.read_s32le()
    parse_generic_object()

junk = b''
# while b'OBST' not in cur.peek(5000):
#     junk += cur.read(5000)
junk += cur.read(288092)
print("junk", len(junk))
if b'\x89\x5a\x08\x4a\x86\x39\x46\x31\x8b\x49\x08\x32\x08\x32\xc4\x10' in junk:
    print("found it!") # hmm. not NornMeso/BLKs then...
    

object_index = cur.read_s32le()
print("object index", object_index)
object_type = cur.read_s32le()
print("object type", object_type) # SkeletalCreature = 0x6
length = cur.read_s32le()
name = cur.read(length) # SkeletalCreature
print("name", name)

parse_generic_object()

    
print(cur.peek(10000))
    
exit()

# print("creaturesarchive tag", cur.read_s32le())


"""

MacroScript -
OBST
u32 0 - ???
u32 family
u32 genus
u32 species
u32 event_number
u32 unknown - 0 - EMIT's command group? or something random?
u32 unknown - 14 - ???
u32 unknown - 107 - EMIT's command index?
u32 unknown - 1 - first argument, a raw integer. how does it know?
u16 unknown - 3 - ? third caos command group? (_P1_ is in group 2, which is the third one...)
u16 unknown - 303 - _P1_'s command index!
u16 unknown - 17 - ???
u32 object index? - 3
u32 object depth? - 1
u32 object magic number four? - 4
OBST
u32 string length
string script text
u32 unknown - 1
u32 unknown - 0
u32 unknown - 0
u32 unknown - 4 - end of object marker?
OBEN
u32 unknown - 4 - end of object marker?
OBEN
"""

# s = (
# b"OBST\x00\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x00\x96\x00\x00\x00\xe8\x03\x00\x00\x00\x00\x00\x00\x0e\x00\x00\x00k\x00\x00\x00\x01\x00\x00\x00\x03\x00/\x01\x11\x00\x03\x00\x00\x00\x01\x00\x00\x00\x04\x00\x00\x00"
#  + b'OBST\x0c\x00\x00\x00emit 1 _p1_ \x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00OBEN'
#  + b'\x04\x00\x00\x00OBEN'
#  )
# 
# print(len(s))
# 
# print(s[:4]) # OBST
# s = s[4:]
# 
# for _ in range(9):
#     print(s[:4], struct.unpack("<i", s[:4])[0])
#     s = s[4:]
# 
# for _ in range(9):
#     print(s[:2], struct.unpack("<h", s[:2])[0])
#     s = s[2:]
# 
# print(s[:4]) # OBST
# s = s[4:]
# 
# print(s[:4], struct.unpack("<i", s[:4])[0])
# length = struct.unpack("<i", s[:4])[0]
# s = s[4:]
# 
# script = s[:length]
# print(script)
# s = s[length:]
# 
# for _ in range(4):
#     print(s[:4], struct.unpack("<i", s[:4])[0])
#     s = s[4:]
# 
# print(s[:4]) # OBEN
# s = s[4:]
# 
# for _ in range(1):
#     print(s[:4], struct.unpack("<i", s[:4])[0])
#     s = s[4:]
# 
# print(s[:4]) # OBEN
# s = s[4:]

# exit()



with open("SpaceAndAllThatIsOutThere.out", "rb") as f:
    # data = f.read()
    
    indent = 0
    buf = b''
    while True:
        # print(f.peek(4))
        if f.peek(4)[:4] == b'OBST':
            print("    " * indent, buf)
            buf = b''
            indent += 1
            buf += f.read(4)
            continue
        elif f.peek(4)[:4] == b'OBEN':
            buf += f.read(4)
            print("    " * indent, buf)
            buf = b''
            indent -= 1
            continue
        buf += f.read(1)
        # print(buf)

    # print("  " * indent, buf)
    
"""    
toplevel, repeated pattern of
\x02\x00\x00\x00 \x00\x00\x00\x00 \x04\x00\x00\x00 OBST
\x04\x00\x00\x00 \x00\x00\x00\x00 \x04\x00\x00\x00 OBST
\x06\x00\x00\x00 \x00\x00\x00\x00 \x04\x00\x00\x00 OBST
\x08\x00\x00\x00 \x00\x00\x00\x00 \x04\x00\x00\x00 OBST
\x0a\x00\x00\x00 \x00\x00\x00\x00 \x04\x00\x00\x00 OBST
\x0c\x00\x00\x00 \x00\x00\x00\x00 \x04\x00\x00\x00 OBST
between objects
 
and an internal pattern of
\x01\x00\x00\x00 \x01\x00\x00\x00 \x04\x00\x00\x00 OBST
\x03\x00\x00\x00 \x01\x00\x00\x00 \x04\x00\x00\x00 OBST

etc..
is it counting total objects, and then nesting level? what is the \x04 for?

also, the first time it happens, we have

\x00\x00\x00\x00 \x0b\0x00\x00\x00 (length of string) MacroScript \x04\x00\x00\x00 OBST

and then inside we have 

\x01\x00\x00\x00 \t\x00\x00\x00 (length of string) DebugInfo \x04\x00\x00\x00 OBST

hmm.

"""