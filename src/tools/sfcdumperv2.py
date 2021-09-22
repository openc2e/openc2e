# codec: utf-8

import json
import re
import sys
import struct


class MFCReader:
    def __init__(self, f):
        self._f = f
        self._classmap = {}
        self._objects = [None]

    def register_class(self, name, klass):
        assert name not in self._classmap
        self._classmap[name] = klass

    def peek(self):
        return self._f.peek()

    def read(self, n):
        result = self._f.read(n)
        assert len(result) == n
        return result

    def read_u8(self):
        return struct.unpack("<B", self.read(1))[0]

    def read_u16le(self):
        return struct.unpack("<H", self.read(2))[0]

    def read_s32le(self):
        return struct.unpack("<i", self.read(4))[0]

    def read_u32le(self):
        return struct.unpack("<I", self.read(4))[0]

    def read_string(self):
        length = self.read_u8()
        if length == 0xFF:
            length = self.read_u16le()
            if length == 0xFFFF:
                length = self.read_u32le()
        return self.read(length)

    def read_object(self, class_requested=None):
        tag = self.read_u16le()
        if tag == 0x7FFF:
            # 32-bit tag
            raise NotImplementedError('32-bit "big" tag')
        if tag == 0:
            return None
        elif tag == 0xFFFF:
            # new class description
            schema_number = self.read_u16le()
            classname_length = self.read_u16le()
            classname = self.read(classname_length).decode("ascii")
            # print(f"{schema_number=} {classname_length=} {classname=}")

            if classname not in self._classmap:
                raise NotImplementedError(
                    'Found new class "{}", but don\'t know how to deserialize it'.format(
                        classname
                    )
                )

            # print('Found new class "{}"'.format(classname))
            self._objects.append(self._classmap[classname])
            # print("Giving it PID {}".format(len(self._objects) - 1))

            val = self._classmap[classname]()
            # print('Found object of type "{}"'.format(classname))
            self._objects.append(val)
            # print("Giving it PID {}".format(len(self._objects) - 1))
            val.read(self)
            return val
        elif tag & 0x8000:
            # existing class
            tag = tag & ~0x8000
            # print("Found object of existing class {}".format(tag))
            assert len(self._objects) >= tag
            val = self._objects[tag]()
            self._objects.append(val)
            # print("Giving it PID {}".format(len(self._objects) - 1))
            val.read(self)
            return val
        else:
            # existing object
            # print("Reference to existing object with PID {}".format(tag))
            assert len(self._objects) >= tag
            return self._objects[tag]


def as_ref(obj):
    if obj:
        return obj._unid
    else:
        return None


class MapData:
    class Room:
        def read(self, ar):
            self.left = ar.read_u32le()
            self.top = ar.read_u32le()
            self.right = ar.read_u32le()
            self.bottom = ar.read_u32le()
            self.roomtype = ar.read_u32le()

        def __json__(self):
            return dict(
                left=self.left,
                top=self.top,
                right=self.right,
                bottom=self.bottom,
                roomtype=self.roomtype,
            )

    class Bacterium:
        def read(self, ar):
            self.state = ar.read_u8()
            self.antigen = ar.read_u8()
            self.fatal_level = ar.read_u8()
            self.infect_level = ar.read_u8()
            self.toxin1 = ar.read_u8()
            self.toxin2 = ar.read_u8()
            self.toxin3 = ar.read_u8()
            self.toxin4 = ar.read_u8()

        def __json__(self):
            return dict(
                state=self.state,
                antigen=self.antigen,
                fatal_level=self.fatal_level,
                infect_level=self.infect_level,
                toxins=[self.toxin1, self.toxin2, self.toxin3, self.toxin4],
            )

    def read(self, ar):
        self.unknown = ar.read_u32le()
        self.timeofday = ar.read_u32le()
        self.background = ar.read_object("CGallery")
        num_rooms = ar.read_u32le()
        # print(f"MapData(unknown={unknown} timeofday={timeofday} num_rooms={num_rooms})")
        self.rooms = []
        for _ in range(num_rooms):
            r = MapData.Room()
            r.read(ar)
            self.rooms.append(r)
            # left = ar.read_u32le()
            # top = ar.read_u32le()
            # right = ar.read_u32le()
            # bottom = ar.read_u32le()
            # roomtype = ar.read_u32le()
            # print(
            #     f"Room(left={left} top={top} right={right} bottom={bottom} roomtype={roomtype})"
            # )

        self.groundlevel = []
        for _ in range(261):
            self.groundlevel.append(ar.read_u32le())
            # print(f"groundlevel={groundlevel}")

        self.bacteria = []
        for _ in range(100):
            b = MapData.Bacterium()
            b.read(ar)
            self.bacteria.append(b)
            # state = ar.read_u8()
            # antigen = ar.read_u8()
            # fatal_level = ar.read_u8()
            # infect_level = ar.read_u8()
            # toxin1 = ar.read_u8()
            # toxin2 = ar.read_u8()
            # toxin3 = ar.read_u8()
            # toxin4 = ar.read_u8()
            # print(
            #     f"Bacterium(state={state} antigen={antigen} fatal_level={fatal_level} infect_level={infect_level} toxins=[{toxin1}, {toxin2}, {toxin3}, {toxin4}])"
            # )

    def __json__(self):
        return dict(
            _type="MapData",
            unknown=self.unknown,
            timeofday=self.timeofday,
            background=self.background,
            rooms=self.rooms,
            groundlevel=self.groundlevel,
            bacteria=self.bacteria,
        )


class CGallery:
    def read(self, ar):
        num_images = ar.read_u32le()
        self.filename = filename = ar.read(4).decode("ascii")
        self.first_sprite = ar.read_u32le()
        num_references = ar.read_u32le()
        # print(
        #     f'CGallery(num_images={num_images} filename="{filename}" first_sprite={self.first_sprite} num_references={num_references})'
        # )
        for _ in range(num_images):
            parent = ar.read_object("CGallery")
            status = ar.read_u8()
            width = ar.read_u32le()
            height = ar.read_u32le()
            offset = ar.read_u32le()
            # print(
            #     f"Image(parent=... status={status} width={width} height={height} offset={offset})"
            # )

    def __json__(self):
        return dict(filename=self.filename, first_sprite=self.first_sprite)


OBJECT_UNID_COUNTER = 1


class Object:
    def __init__(self):
        global OBJECT_UNID_COUNTER
        self._unid = OBJECT_UNID_COUNTER
        OBJECT_UNID_COUNTER += 1

    def read(self, ar):
        unused = ar.read_u8()
        self.species = ar.read_u8()
        self.genus = ar.read_u8()
        self.family = ar.read_u8()
        self.movement_status = ar.read_u8()
        self.attr = ar.read_u8()
        self.limit_left = ar.read_u32le()
        self.limit_top = ar.read_u32le()
        self.limit_right = ar.read_u32le()
        self.limit_bottom = ar.read_u32le()
        self.carrier = ar.read_object("Object")
        self.actv = ar.read_u8()
        self.gallery = ar.read_object("CGallery")
        self.tick_time = ar.read_u32le()
        self.tick_state = ar.read_u32le()
        self.objp = ar.read_object("Object")
        self.current_sound_filename = ar.read(4).decode("ascii")
        self.current_sound_filename = self.current_sound_filename[
            : self.current_sound_filename.find("\x00")
        ]
        self.obv0 = ar.read_u32le()
        self.obv1 = ar.read_u32le()
        self.obv2 = ar.read_u32le()
        num_scripts = ar.read_u32le()
        for _ in range(num_scripts):
            script_eventno = ar.read_u8()
            script_species = ar.read_u8()
            script_genus = ar.read_u8()
            script_family = ar.read_u8()
            script_text = ar.read_string().decode("ascii")
        pass

    def __json__(self):
        return dict(
            _type="Object",
            _unid=self._unid,
            family=self.family,
            genus=self.genus,
            species=self.species,
            movement_status=self.movement_status,
            attr=self.attr,
            limit=[
                self.limit_left,
                self.limit_top,
                self.limit_right,
                self.limit_bottom,
            ],
            carrier=as_ref(self.carrier),
            actv=self.actv,
            gallery=self.gallery,
            tick_time=self.tick_time,
            tick_state=self.tick_state,
            objp=as_ref(self.objp),
            current_sound=self.current_sound_filename,
            obv=[self.obv0, self.obv1, self.obv2],
        )


class Scenery(Object):
    def read(self, ar):
        Object.read(self, ar)
        self.part = ar.read_object("Entity")

    def __json__(self):
        return {**Object.__json__(self), "_type": "Scenery", "part": self.part}


class SimpleObject(Object):
    def read(self, ar):
        Object.read(self, ar)
        self.part = ar.read_object("Entity")
        self.zorder = ar.read_u32le()
        self.click_bhvr = []
        self.click_bhvr.append(ar.read_u8())
        self.click_bhvr.append(ar.read_u8())
        self.click_bhvr.append(ar.read_u8())
        self.touch_bhvr = ar.read_u8()

    def __json__(self):
        return {
            **Object.__json__(self),
            "_type": "SimpleObject",
            "part": self.part,
            "zorder": self.zorder,
            "click_bhvr": self.click_bhvr,
            "touch_bhvr": self.touch_bhvr,
        }


class PointerTool(SimpleObject):
    def read(self, ar):
        SimpleObject.read(self, ar)
        relx = ar.read_u32le()
        rely = ar.read_u32le()
        bubble = ar.read_object("Bubble")
        text = ar.read(25)

    def __json__(self):
        return {**SimpleObject.__json__(self), "_type": "PointerTool", "hello": 1}


class CallButton(SimpleObject):
    def read(self, ar):
        SimpleObject.read(self, ar)
        self.lift = ar.read_object("Lift")
        self.button_id = ar.read_u8()

    def __json__(self):
        return {
            **SimpleObject.__json__(self),
            "_type": "CallButton",
            "lift": as_ref(self.lift),
            "button_id": self.button_id,
        }


class CompoundObject(Object):
    def read(self, ar):
        Object.read(self, ar)
        num_parts = ar.read_u32le()
        print(f"{num_parts=}")
        for _ in range(num_parts):
            part = ar.read_object("Entity")
            part_x = ar.read_u32le()
            part_y = ar.read_u32le()

        for _ in range(6):
            hotspot_left = ar.read_u32le()
            hotspot_top = ar.read_u32le()
            hotspot_right = ar.read_u32le()
            hotspot_bottom = ar.read_u32le()

        for _ in range(6):
            function_hotspot = ar.read_u32le()


class Blackboard(CompoundObject):
    def read(self, ar):
        CompoundObject.read(self, ar)
        background_color = ar.read_u8()
        chalk_color = ar.read_u8()
        alias_color = ar.read_u8()
        text_x = ar.read_u8()
        text_y = ar.read_u8()
        for _ in range(16):
            word_value = ar.read_s32le()
            word_text = ar.read(11)
            word_text = word_text[: word_text.find(b"\x00")].decode("ascii")
            print(f"Word(value={word_value} text={word_text})")


class Vehicle(CompoundObject):
    def read(self, ar):
        CompoundObject.read(self, ar)
        velx_fp = ar.read_u32le()
        vely_fp = ar.read_u32le()
        coordx_fp = ar.read_u32le()
        coordy_fp = ar.read_u32le()
        cabin_left = ar.read_u32le()
        cabin_top = ar.read_u32le()
        cabin_right = ar.read_u32le()
        cabin_bottom = ar.read_u32le()
        bump = ar.read_u32le()


class Lift(Vehicle):
    def read(self, ar):
        Vehicle.read(self, ar)
        num_floors = ar.read_u32le()
        next_or_current_floor = ar.read_u32le()
        current_call_button = ar.read_s32le()
        delay_counter = ar.read_u8()
        for _ in range(8):
            floor_y = ar.read_u32le()
            floor_callbutton = ar.read_object("CallButton")


class Entity:
    def read(self, ar):
        self.gallery = ar.read_object("CGallery")
        self.current_sprite = ar.read_u8()
        self.image_offset = ar.read_u8()
        self.zorder = ar.read_u32le()
        self.x = ar.read_u32le()
        self.y = ar.read_u32le()
        self.has_animation = ar.read_u8()
        if self.has_animation:
            self.animation_frame = ar.read_u8()
            self.animation_string = ar.read(32)
            self.animation_string = self.animation_string[
                : self.animation_string.find(b"\x00")
            ].decode("ascii")
        else:
            self.animation_frame = 0
            self.animation_string = ""

        # print(
        # f"Entity(gallery={self.gallery.filename} current_sprite={self.current_sprite} image_offset={self.image_offset} zorder={self.zorder} x={self.x} y={self.y} has_animation={has_animation} animation_frame={animation_frame} animation_string={animation_string})"
        # )

    def __json__(self):
        return dict(
            _type="Entity",
            gallery=self.gallery,
            current_sprite=self.current_sprite,
            image_offset=self.image_offset,
            zorder=self.zorder,
            x=self.x,
            y=self.y,
            has_animation=self.has_animation,
            animation_frame=self.animation_frame,
            animation_string=self.animation_string,
        )


class Macro:
    def read(self, ar):
        self.selfdestruct = ar.read_u32le()
        self.inst = ar.read_u32le()
        self.maybe_text_length = ar.read_u32le()
        self.script = ar.read_string().decode("ascii")
        self.ip = ar.read_u32le()
        self.stack = []
        for _ in range(20):
            self.stack.append(ar.read_u32le())
        self.sp = ar.read_u32le()
        self.vars = []
        for _ in range(10):
            self.vars.append(ar.read_u32le())

        self.ownr = ar.read_object("Object")
        self.from_ = ar.read_object("Object")
        self.exec_ = ar.read_object("Object")
        self.targ = ar.read_object("Object")
        self._it_ = ar.read_object("Object")
        self.part = ar.read_u32le()
        self.label_most_recently_visited_subroutine = ar.read(4)
        self.label_most_recently_visited_subroutine = (
            self.label_most_recently_visited_subroutine[
                : self.label_most_recently_visited_subroutine.find(b"\x00")
            ].decode("ascii")
        )
        self.address_most_recently_visited_subroutine = ar.read_u32le()
        self.wait = ar.read_u32le()

    def __json__(self):
        return dict(
            _type="Macro",
            selfdestruct=self.selfdestruct,
            inst=self.inst,
            maybe_text_length=self.maybe_text_length,
            script=self.script,
            ip=self.ip,
            stack=self.stack,
            sp=self.sp,
            vars=self.vars,
            ownr=as_ref(self.ownr),
            from_=as_ref(self.from_),
            exec_=as_ref(self.exec_),
            targ=as_ref(self.targ),
            _it_=as_ref(self._it_),
            part=self.part,
            label_most_recently_visited_subroutine=self.label_most_recently_visited_subroutine,
            address_most_recently_visited_subroutine=self.address_most_recently_visited_subroutine,
            wait=self.wait,
        )


#
# class MyJSONEncoder(json.JSONEncoder):
#     def default(self, obj):
#         if isinstance(obj, (MapData, CGallery, Entity, Object, Macro)):
#             return obj.__json__()
#         return json.JSONEncoder.default(self, obj)

class Body(Entity):
    def read(self, ar):
        Entity.read(self, ar)
        
        angle = ar.read_u32le()
        view = ar.read_u32le()
        
        body_data = []
        for i in range(6):
            val = []
            for j in range(10):
                x = ar.read_u8()
                y = ar.read_u8()
                val.append((x, y))
            body_data.append(val)
    
        print(vars())

class Limb(Entity):
    class LimbData:
        def read(self, ar):
            self.startx = ar.read_u8()
            self.starty = ar.read_u8()
            self.endx = ar.read_u8()
            self.endy = ar.read_u8()
    
    def read(self, ar):
        Entity.read(self, ar)
        
        angle = ar.read_u32le()
        view = ar.read_u32le()
        
        limbdata = []
        for _ in range(10):
            val = self.LimbData()
            val.read(ar)
            limbdata.append(limbdata)
            
        next_limb = ar.read_object("Limb")
        
        print(vars())

class CBrain:        
    def read(self, ar):
        num_lobes = ar.read_u32le()
        lobe_headers = []
        for _ in range(num_lobes):
            lobe_headers.append(self.LobeHeader())
            lobe_headers[-1].read(ar)
            
        neurons = []
        for lobe in lobe_headers:
            for _ in range(lobe.num_cells):
                neurons.append(self.Neuron())
                neurons[-1].read(ar)
    
    class SVRule:
        @classmethod
        def from_read(cls, ar):
            val = cls()
            val.read(ar)
            return val
            
        def read(self, ar):
            self.data = ar.read(10)
        
        def __repr__(self):
            
            svrules = {
                0: 'blank',
                1: 'const0',
                2: 'const1',
                3: 'const64',
                4: 'const255',
                5: 'chem0',
                6: 'chem1',
                7: 'chem2',
                8: 'chem3',
                9: 'state',
                10: 'output',
                11: 'thres',
                12: 'type0',
                13: 'type1',
                14: 'andtype0',
                15: 'andtype1',
                16: 'input',
                17: 'conduct',
                18: 'suscept',
                19: 'stw',
                20: 'ltw',
                21: 'strength',
                # hmm, C2 adds some extra codes here. assuming C1 doesn't have
                # empty rules that were "for future use".
                22: 'TRUE',
                23: 'PLUS',
                24: 'MINUS',
                25: 'TIMES',
                26: 'INCR',
                27: 'DECR',
                # C2 adds extra ops here too
            }
            
            res = []
            for op in self.data:
                if op == 0:
                    # TODO: if rest aren't blank then show them anyways?
                    break
                elif op in svrules:
                    res.append(svrules[op])
                else:
                    res.append(str(op))
                    raise NotImplementedError(op)
            
            return repr(":".join(res))
            


	# // entries from here on are opcodes for use in rules
	# SV_TRUE,				// if v is zero, abort rule, else continue (MUST BE FIRST OPCODE! IN THIS LIST!)
	# SV_PLUS,				// v = v + next operand
	# SV_MINUS,				// v = v - next operand
	# SV_TIMES,				// v = v * next operand as a fraction (modulate)
	# SV_INCR,				// v++
	# SV_DECR,				// v--
    # 
    # // C2 additions.
    # SV_FALSE,			    // if v is non-zero, abort rule, else continue
	# SV_MULTIPLY,            // v = v * next operand
    # SV_AVERAGE,             // v = (v + next operand) / 2
    # SV_MOVETOWARDS,         // v = v + ((next operand - v) / (next operand / 255))
    # SV_RND,                 // v = random(next operand, next operand) [Sets SVCONSTRAND too!]
    # 
	# NUMSV					// (number of entries in a SV table)
    
    class LobeHeader:        
        def read(self, ar):
            self.x = ar.read_u32le()
            self.y = ar.read_u32le()
            self.width = ar.read_u32le()
            self.height = ar.read_u32le()
            
            self.perceptible = ar.read_u32le() # 0 = no 1 = yes 2 = mutually exclusive
            self.activity = ar.read_u8()
            self.chemicals = []
            for _ in range(4):
                self.chemicals.append(ar.read_u8())
            
            self.cell_threshold = ar.read_u8()
            self.cell_leakage = ar.read_u8()
            self.cell_reststate = ar.read_u8()
            self.cell_inputgain = ar.read_u8()
            self.cell_svrule = CBrain.SVRule.from_read(ar)
            self.cell_flags = ar.read_u8()
            
            self.dendritetype1 = self.DendriteType()
            self.dendritetype1.read(ar)
            self.dendritetype2 = self.DendriteType()
            self.dendritetype2.read(ar)
                
            self.num_cells = ar.read_u32le()
            assert self.num_cells == self.width * self.height
            self.num_dendrites = ar.read_u32le()
            
            print(vars(self))
        
        def __repr__(self):
            return f"{vars(self)}"
        
        class DendriteType:
            def read(self, ar):
                self.source = ar.read_u32le()
                self.min = ar.read_u8()
                self.max = ar.read_u8()
                self.spread = ar.read_u8()
                self.fanout = ar.read_u8()
                self.minltw = ar.read_u8()
                self.maxltw = ar.read_u8()
                self.minstrength = ar.read_u8()
                self.maxstrength = ar.read_u8()
                self.migration = ar.read_u8()
                self.relaxsuscept = ar.read_u8()
                self.relaxstw = ar.read_u8()
                self.ltwgainrate = ar.read_u8()
                self.gainevery = ar.read_u8()
                self.loseevery = ar.read_u8()
                
                # the right order? or do suscept and reinforce come first?
                self.gainrule = CBrain.SVRule.from_read(ar)
                self.loserule = CBrain.SVRule.from_read(ar)
                self.susceptrule = CBrain.SVRule.from_read(ar)
                self.reinforcerule = CBrain.SVRule.from_read(ar)
            
            def __repr__(self):
                return f"{vars(self)}"

    class Neuron:
        def read(self, ar):
            self.x = ar.read_u8()
            self.y = ar.read_u8()
            self.output = ar.read_u8()
            self.state = ar.read_u8()
            self.wta_disabled = ar.read_u8() # maybe is a u4?
            self.unknown = ar.read_u8()
            
            self.d0_number = ar.read_u8()
            self.d0_type = ar.read_u32le()
            self.d0_dendrites = []
            for _ in range(self.d0_number):
                self.d0_dendrites.append(self.Dendrite())
                self.d0_dendrites[-1].read(ar)
            
            self.d1_number = ar.read_u8()
            self.d1_type = ar.read_u32le()
            self.d1_dendrites = []
            for _ in range(self.d1_number):
                self.d1_dendrites.append(self.Dendrite())
                self.d1_dendrites[-1].read(ar)
            
            print(vars(self))
        
        def __repr__(self):
            return f"{vars(self)}"
        
        class Dendrite:
            def read(self, ar):
                self.source = ar.read_u32le()
                self.x = ar.read_u8()
                self.y = ar.read_u8()
                self.suscept = ar.read_u8()
                self.stw = ar.read_u8()
                self.ltw = ar.read_u8()
                self.strength = ar.read_u8()
            
            def __repr__(self):
                return f"{vars(self)}"
            
class CBiochemistry:
    def read(self, ar):
        self.owner = ar.read_object("Creature")
        
        num_emitters = ar.read_u32le()
        num_receptors = ar.read_u32le()
        num_reactions = ar.read_u32le()
        
        self.chemicals = []
        for _ in range(256):
            self.chemicals.append(self.ChemicalData.read_from(ar))
        
        self.emitters = []
        for _ in range(num_emitters):
            self.emitters.append(self.Emitter.read_from(ar))
        
        print(vars(self))
        
        self.receptors = []
        for _ in range(num_receptors):
            self.receptors.append(self.Receptor.read_from(ar))
        
        self.reactions = []
        for _ in range(num_reactions):
            self.reactions.append(self.Reaction.read_from(ar))
    
    class ChemicalData:
        @classmethod
        def read_from(cls, ar):
            val = cls()
            val.read(ar)
            return val
        
        def read(self, ar):
            self.concentration = ar.read_u8()
            self.decay = ar.read_u8()
        
        def __repr__(self):
            return repr(vars(self))
    
    class Emitter:
        @classmethod
        def read_from(cls, ar):
            val = cls()
            val.read(ar)
            return val
        
        def read(self, ar):
            self.organ = ar.read_u8()
            self.tissue = ar.read_u8()
            self.locus = ar.read_u8()
            self.chemical = ar.read_u8()
            self.threshold = ar.read_u8()
            self.rate = ar.read_u8()
            self.gain = ar.read_u8()
            self.effect = ar.read_u8()
        
        def __repr__(self):
            return repr(vars(self))
    
    class Receptor:
        @classmethod
        def read_from(cls, ar):
            val = cls()
            val.read(ar)
            return val
        
        def read(self, ar):
            self.organ = ar.read_u8()
            self.tissue = ar.read_u8()
            self.locus = ar.read_u8()
            self.chemical = ar.read_u8()
            self.threshold = ar.read_u8()
            self.nominal = ar.read_u8()
            self.gain = ar.read_u8()
            self.effect = ar.read_u8()
        
        def __repr__(self):
            return repr(vars(self))
    
    class Reaction:
        @classmethod
        def read_from(cls, ar):
            val = cls()
            val.read(ar)
            return val
        
        def read(self, ar):
            self.r1_amount = ar.read_u8()
            self.r1_chem = ar.read_u8()
            self.r2_amount = ar.read_u8()
            self.r2_chem = ar.read_u8()
            self.rate = ar.read_u8()
            self.p1_amount = ar.read_u8()
            self.p1_chem = ar.read_u8()
            self.p2_amount = ar.read_u8()
            self.p2_chem = ar.read_u8()
        
        def __repr__(self):
            return repr(vars(self))
        
class CInstinct:
    def read(self, ar):
        self.lobe0 = ar.read_u32le()
        self.cell0 = ar.read_u32le()
        self.lobe1 = ar.read_u32le()
        self.cell1 = ar.read_u32le()
        self.lobe2 = ar.read_u32le()
        self.cell2 = ar.read_u32le()
        self.action = ar.read_u32le()
        self.reinforcement_chemical = ar.read_u32le()
        self.reinforcement_amount = ar.read_u32le()
        self.phase = ar.read_u32le()
        # 
        # print(vars(self))
        # exit()
    
    def __repr__(self):
        return repr(vars(self))

class Creature(Object):
    class CreatureVocabWord:
        def read(self, ar):
            self.in_ = ar.read_string()
            self.out = ar.read_string()
            self.strength = ar.read_u32le()
        
        def __repr__(self):
            return f"{{in={self.in_} out={self.out} strength={self.strength}}}"
    
    class Stimulus:
        def read(self, ar):
            self.significance = ar.read_u8()
            self.input = ar.read_u8()
            self.intensity = ar.read_u8()
            self.features = ar.read_u8()
            self.chemical0 = ar.read_u8()
            self.amount0 = ar.read_u8()
            self.chemical1 = ar.read_u8()
            self.amount1 = ar.read_u8()
            self.chemical2 = ar.read_u8()
            self.amount2 = ar.read_u8()
            self.chemical3 = ar.read_u8()
            self.amount3 = ar.read_u8()
    
    class Goal:
        def read(self, ar):
            self.drives = []
            for _ in range(16):
                self.drives.append(ar.read_u32le())
        
        def __repr__(self):
            return f"{self.drives}"
    
    def read(self, ar):
        Object.read(self, ar)
        
        moniker = ar.read(4)
        mothers_moniker = ar.read(4)
        fathers_moniker = ar.read(4)

        body = ar.read_object("Body")
        head = ar.read_object("Limb")
        left_thigh = ar.read_object("Limb")
        right_thigh = ar.read_object("Limb")
        left_arm = ar.read_object("Limb")
        right_arm = ar.read_object("Limb")
        tail = ar.read_object("Limb")
        
        direction = ar.read_u8()
        downfoot = ar.read_u8()
        footx = ar.read_u32le()
        footy = ar.read_u32le()
        zorder = ar.read_u32le()
        
        current_pose = ar.read_string()
        expression = ar.read_u8()
        eyes_open = ar.read_u8()
        asleep = ar.read_u8()
        
        poses = []
        for _ in range(100):
            poses.append(ar.read_string())
            
        gait_animations = []
        for _ in range(8):
            gait_animations.append(ar.read_string())
        
        vocabulary = []
        for _ in range(80):
            vocabulary.append(self.CreatureVocabWord())
            vocabulary[-1].read(ar)
            
        remembered_object_positions = []
        for _ in range(40):
            x = ar.read_u32le()
            y = ar.read_u32le()
            remembered_object_positions.append((x, y))
        
        stimuli = []
        for _ in range(36):
            stimuli.append(self.Stimulus())
            stimuli[-1].read(ar)
        
        brain = ar.read_object("Brain")
        biochemistry = ar.read_object("Biochemistry")
        
        self.sex = ar.read_u8()
        self.age = ar.read_u8()
        self.biological_tick = ar.read_u32le()
        
        self.gamete = ar.read(4)
        self.zygote = ar.read(4)
        
        self.dead = ar.read_u8()
        self.age_in_ticks = ar.read_u32le()
        
        number_of_instincts = ar.read_u32le()
        self.dreaming = ar.read_u32le()
        self.instincts = []
        for _ in range(number_of_instincts):
            self.instincts.append(ar.read_object("CInstinct"))
        
            
        self.goals = []
        for _ in range(40):
            self.goals.append(self.Goal())
            self.goals[-1].read(ar)
        
        print(vars(self))
        
        
        
        zzzz = ar.read_object("Object")
        
        
        
        # stars = ar.read_object("Object")
        # bubbles = ar.read_object("Object")
        # 
        # status_of_hand_before_disregarded = ar.read_u32le()
        
        # voice_lookup = ar.read(4 * 27 * 3)
        # voice_data = ar.read(8 * 32)
        
        
        
        voice_lookup_table = []
        for _ in range(27 * 3):
            voice_lookup_table.append(ar.read_u32le())
        voices = []
        for _ in range(32):
            name = ar.read(4)
            delay_ticks = ar.read_u32le()
            voices.append((name, delay_ticks))
            
        self.history_moniker = ar.read_string()
        self.history_name = ar.read_string()
        self.history_moms_moniker = ar.read_string()
        # self.history_moms_name = ar.read_string()
        self.history_dads_moniker = ar.read_string()
        # self.history_dads_name = ar.read_string()
        self.history_birthday = ar.read_string()
        self.history_birthplace = ar.read_string()
        
        self.history_owner_name = ar.read_string()
        self.history_owner_phone = ar.read_string()
        self.history_owner_address = ar.read_string()
        self.history_owner_email = ar.read_string()
        # self.history_state = ar.read_u16le()
        # self.history_gender = ar.read_u16le()
        # self.history_age_in_seconds = ar.read_u32le()
        
        
        
        print(vars(self))
        
        # padding ???
        
        # print(vars())

class BufferReader:
    def __init__(self, buf):
        self._buf = buf
        self._p = 0
    
    def read(self, n):
        result = self._buf[self._p:self._p+n]
        self._p += n
        return result

class Gene:
    class GeneHeader:
        def read(self, ar):
            self.type = ar.read_u8()
            self.sub_type = ar.read_u8()
            
            # self.unknown = [ar.read_u8(), ar.read_u8(), ar.read_u8(), ar.read_u8()]
            
            self.gene_id = ar.read_u8()
            self.generation = ar.read_u8()
            self.switchontime = ar.read_u8()
            self.flags = ar.read_u8()
            # self.mutability_weighting = ar.read_u8()
        
        def __repr__(self):
            return repr(vars(self))
    
    @classmethod
    def read_from(cls, ar):
        val = cls()
        val.read(ar)
        return val
    
    def read(self, ar):
        self.header = self.GeneHeader()
        self.header.read(ar)
        type = (self.header.type, self.header.sub_type)
        
        if type == (0, 0):
            self.type_name = "Brain Lobe"
            self.x = ar.read_u8()
            self.y = ar.read_u8()
            self.width = ar.read_u8()
            self.height = ar.read_u8()
            self.perceptible = ar.read_u8()
            
            self.cell_threshold = ar.read_u8()
            self.cell_leakage = ar.read_u8()
            self.cell_reststate = ar.read_u8()
            self.cell_attenuation = ar.read_u8()
            self.cell_staterule = ar.read(8)
            self.cell_flags = ar.read_u8()
            
            class DenType:
                def read(self, ar):
                    self.source = ar.read_u8()
                    self.min = ar.read_u8()
                    self.max = ar.read_u8()
                    self.spread = ar.read_u8()
                    self.fanout = ar.read_u8()
                    self.minltw = ar.read_u8()
                    self.maxltw = ar.read_u8()
                    self.minstrength = ar.read_u8()
                    self.maxstrength = ar.read_u8()
                    self.migration = ar.read_u8()
                    self.relaxsuscept = ar.read_u8()
                    self.relaxstw = ar.read_u8()
                    self.ltwgainrate = ar.read_u8()
                    self.gainevery = ar.read_u8()
                    self.gainrule = ar.read(8)
                    self.loseevery = ar.read_u8()
                    self.loserule = ar.read(8)
                    self.susceptrule = ar.read(8)
                    self.reinforcerule = ar.read(8)
                
                def __repr__(self):
                    return repr(vars(self))
            
            self.d0_type = DenType()
            self.d0_type.read(ar)
            self.d1_type = DenType()
            self.d1_type.read(ar)
        elif type == (1, 0):
            self.type_name = "Biochemistry Receptor"
            self.organ = ar.read_u8()
            self.tissue = ar.read_u8()
            self.locus = ar.read_u8()
            self.chemical = ar.read_u8()
            self.threshold = ar.read_u8()
            self.nominal = ar.read_u8()
            self.gain = ar.read_u8()
            self.flags = ar.read_u8()
        elif type == (1, 1):
            self.type_name = "Biochemistry Emitter"
            self.organ = ar.read_u8()
            self.tissue = ar.read_u8()
            self.locus = ar.read_u8()
            self.chemical = ar.read_u8()
            self.threshold = ar.read_u8()
            self.rate = ar.read_u8()
            self.gain = ar.read_u8()
            self.flags = ar.read_u8()
        elif type == (1, 2):
            self.type_name = "Biochemistry Reaction"
            self.r1_amount = ar.read_u8()
            self.r1_chem = ar.read_u8()
            self.r2_amount = ar.read_u8()
            self.r2_chem = ar.read_u8()
            self.p1_amount = ar.read_u8()
            self.p1_chem = ar.read_u8()
            self.p2_amount = ar.read_u8()
            self.p2_chem = ar.read_u8()
            self.rate = ar.read_u8()
        elif type == (1, 3):
            self.type_name = "Biochemistry Halflife"
            self.halflifes = []
            for _ in range(256):
                self.halflifes.append(ar.read_u8())
        elif type == (1, 4):
            self.type_name = "Biochemistry Initial Concentrations"
            self.chemical = ar.read_u8()
            self.amount = ar.read_u8()
        elif type == (2, 0):
            self.type_name = "Creature Stimulus"
            self.stimulus = ar.read_u8()
            self.significance = ar.read_u8()
            self.input = ar.read_u8()
            self.intensity = ar.read_u8()
            self.features = ar.read_u8()
            self.chemical0 = ar.read_u8()
            self.amount0 = ar.read_u8()
            self.chemical1 = ar.read_u8()
            self.amount1 = ar.read_u8()
            self.chemical2 = ar.read_u8()
            self.amount2 = ar.read_u8()
            self.chemical3 = ar.read_u8()
            self.amount3 = ar.read_u8()
        elif type == (2, 1):
            self.type_name = "Creature Genus"
            self.genus = ar.read_u8() # wraps to 0–3
            self.moms_moniker = ar.read(4)
            self.dads_moniker = ar.read(4)
        elif type == (2, 2):
            self.type_name = "Creature Appearance"
            self.body_part = ar.read_u8()
            self.variant = ar.read_u8()
        elif type == (2, 3):
            self.type_name = "Creature Pose"
            self.pose_number = ar.read_u8()
            self.pose_string = ar.read(15)
        elif type == (2, 4):
            self.type_name = "Creature Gait"
            self.gait_number = ar.read_u8()
            self.pose_sequence = []
            for _ in range(8):
                self.pose_sequence.append(ar.read_u8())
        elif type == (2, 5):
            self.type_name = "Creature Instinct"
            self.lobe0 = ar.read_u8()
            self.cell0 = ar.read_u8()
            self.lobe1 = ar.read_u8()
            self.cell1 = ar.read_u8()
            self.lobe2 = ar.read_u8()
            self.cell2 = ar.read_u8()
            self.action = ar.read_u8()
            self.reinforcement_chemical = ar.read_u8()
            self.reinforcement_amount = ar.read_u8()
        elif type == (2, 6):
            self.type_name = "Creature Pigments"
            self.pigment_color = ar.read_u8()
            self.amount = ar.read_u8()
        else:
            raise NotImplementedError(type)
    
    def __repr__(self):
        return repr(vars(self))

def parse_genfile(f):
    r = MFCReader(f)
    
    genes = []
    while True:
        marker = r.read(4)
        if marker == b'gend':
            break
        if not marker == b'gene':
            raise NotImplementedError(marker)
        assert marker == b'gene'
        genes.append(Gene.read_from(r))
        
        print(genes[-1])
    
    return genes

class CGenome:
    def read(self, ar):
        length_in_bytes = ar.read_u32le()
        self.moniker = ar.read(4)
        self.sex = ar.read_u32le()
        self.life_stage = ar.read_u8()
        
        print(vars(self))
        
        data = ar.read(length_in_bytes)
        self.genes = parse_genfile(BufferReader(data))
        
        # r = MFCReader(BufferReader(data))
        # 
        # self.genes = []
        # while True:
        #     marker = r.read(4)
        #     if marker == b'gend':
        #         break
        #     if not marker == b'gene':
        #         raise NotImplementedError(marker)
        #     assert marker == b'gene'
        #     self.genes.append(self.Gene.read_from(r))
        
        

def read_exp_file(ar):
    creature = ar.read_object("Creature")
    genome = ar.read_object("CGenome")
    if creature.zygote != b'\x00\x00\x00\x00':
        zygote = ar.read_object("CGenome")
    # TODO: assert eof
    # zygote = ar.read_object("CGenome")
    
    assert ar.peek() == b''


def my_default(obj):
    if isinstance(obj, type):
        return repr(obj)
    return obj.__json__()

def read_sfc_file(ar):
    map = ar.read_object("MapData")
    print(json.dumps(map, indent=1, default=my_default))

    num_objects = ar.read_u32le()
    for _ in range(num_objects):
        obj = ar.read_object("Object")
        print(json.dumps(obj, indent=1, default=my_default))
        # print(MyJSONEncoder().encode(obj, indent=1))
        # exit()

    num_sceneries = ar.read_u32le()
    for _ in range(num_sceneries):
        scen = ar.read_object("Scenery")
        print(json.dumps(scen, indent=1, default=my_default))

    num_scripts = ar.read_u32le()
    for _ in range(num_scripts):
        script_eventno = ar.read_u8()
        script_species = ar.read_u8()
        script_genus = ar.read_u8()
        script_family = ar.read_u8()
        script_text = ar.read_string().decode("ascii")
        print(
            f"scrp {script_family} {script_genus} {script_species} {script_eventno},{script_text}"
        )

    scrollx = ar.read_u32le()
    scrolly = ar.read_u32le()
    current_norn = ar.read_object("Creature")
    for _ in range(6):
        favoriteplace_name = ar.read_string()
        favoriteplace_x = ar.read_u16le()
        favoriteplace_y = ar.read_u16le()
        print(f"{favoriteplace_name=} {favoriteplace_x=} {favoriteplace_y=}")

    num_speech_history = ar.read_u16le()
    print(f"{num_speech_history=}")
    for _ in range(num_speech_history):
        speechhistory_text = ar.read_string()

    num_macros = ar.read_u32le()
    for _ in range(num_macros):
        macro = ar.read_object("Macro")
        print(json.dumps(macro, indent=1, default=my_default))


def main():
    if len(sys.argv) == 1:
        sys.stderr.write(f"USAGE: {sys.argv[0]} FILE\n")
        sys.exit(1)
        
        
    for filename in sys.argv[1:]:
        with open(filename, "rb") as f:
            ar = MFCReader(f)
            
            ar.register_class("MapData", MapData)
            ar.register_class("CGallery", CGallery)
            ar.register_class("Entity", Entity)
            ar.register_class("Object", Object)
            ar.register_class("Scenery", Scenery)
            ar.register_class("SimpleObject", SimpleObject)
            ar.register_class("PointerTool", PointerTool)
            ar.register_class("CompoundObject", CompoundObject)
            ar.register_class("Vehicle", Vehicle)
            ar.register_class("Lift", Lift)
            ar.register_class("Blackboard", Blackboard)
            ar.register_class("CallButton", CallButton)
            ar.register_class("Macro", Macro)
            
            ar.register_class("Creature", Creature)
            ar.register_class("Body", Body)
            ar.register_class("Limb", Limb)
            ar.register_class("CInstinct", CInstinct)
            ar.register_class("CBrain", CBrain)
            ar.register_class("CBiochemistry", CBiochemistry)
            ar.register_class("CGenome", CGenome)

            if f.peek(14)[:14] == b'\xff\xff\x01\x00\x08\x00Creature':
                read_exp_file(ar)
            elif f.peek(13)[:13] == b'\xff\xff\x01\x00\x07\x00MapData':
                read_sfc_file(ar)
            else:
                print("Unknown file type")

    sys.exit(0)


if __name__ == "__main__":
    main()
