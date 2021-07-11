# codec: utf-8

import struct

class cursor:
    def __init__(self, buf):
        self._buf = buf
        self._p = 0
    
    def read(self, n):
        val = self._buf[self._p : self._p + n]
        assert(len(val) == n)
        self._p += n
        return val
    
    def read_s32le(self):
        return struct.unpack("<i", self.read(4))[0]
    
    def read_string(self):
        length = self.read_s32le()
        return self.read(length)

with open('caos.syntax.out', 'rb') as f:
    data = f.read()

cur = cursor(data)

creaturesarchivemarker = cur.read_s32le()
assert(creaturesarchivemarker == 0x27)

caossyntaxmarker = cur.read_s32le()
assert(caossyntaxmarker == 0x6)

version = cur.read_string()
print(version)

num_categories = cur.read_s32le()
print("num_categories=", num_categories)

for i in range(num_categories):
    category = cur.read_string()
    print("category", i, ":", category)

num_script_names = cur.read_s32le()
print("num_script_names=", num_script_names)
for i in range(num_script_names):
    script_name = cur.read_string()
    print("script name {}:".format(i), script_name)

def read_unknown_s32le(cur_, expected):
    if type(expected) not in (tuple, list):
        expected = (expected,)
    unknown = cur_.read_s32le()
    print("unknown = {} ({:#x})".format(unknown, unknown))
    if unknown not in expected:
        s = ["{} ({:#x})".format(_, _) for _ in expected]
        if len(s) > 1:
            s[-1] = "or " + s[-1]
        expected_string = ', '.join(s)
        raise Exception("Expected {}; but got {} ({:#x}) - next few bytes: {}".format(expected_string, unknown, unknown, cur_.read(15)))

number_of_command_groups = cur.read_s32le()
print("number of command groups", number_of_command_groups)

def read_command_list(command_group_id):
    print("command group id {} ({:#x})".format(command_group_id, command_group_id))
    number = cur.read_s32le()
    print("number of commands", number)
    print("")
    for i in range(number):
        print("i", i)
        command_id = cur.read_s32le()
        print("command id: ", command_id)
        
        command_name = cur.read_string()
        print("command: ", command_name)
        
        command_signature = cur.read_string()
        print("signature: ", command_signature)
        
        control_flow_id = cur.read_s32le()
        print("control flow id:", control_flow_id)

        supercommand_id = cur.read_s32le()
        print('supercommand id:', supercommand_id)
        if command_signature != b'*':
            assert(supercommand_id == 0)
        
        argument_names = cur.read_string().split(b' ')
        if argument_names == [b'']:
            argument_names = []
        if command_signature == b'*':
            assert(argument_names == [b'subcommand'])
        elif command_signature == b'-c':
            assert(argument_names == [b'condition'])
        else:
            needed_arguments = len(command_signature)
            if command_signature.startswith(b'-'): # seems to mean special handling/control flow?
                needed_arguments -= 1
            assert(len(argument_names) == needed_arguments)
        print("argument_names", argument_names)
        
        category = cur.read_s32le()
        print("category:", category)
        
        description = cur.read_string()
        print("description:", description)
        
        read_unknown_s32le(cur, command_group_id)
        print("")

# toplevel commands
read_command_list(0x0)

# commands returning ints
read_command_list(0x1)

# commands returning variable
read_command_list(0x2)

# commands returning float
read_command_list(0x3)

# commands returning string
read_command_list(0x4)

# commands returning agents
read_command_list(0x5)

# a bunch of subcommands
for i in range(0x6, number_of_command_groups):
    read_command_list(i)

assert(cur._buf[cur._p:] == b'')
