#!/usr/bin/python
# sfcdumper
# a utility to extract information from Creatures 1/2 SFC (world save) files
# reverse-engineered by fuzzie and nornagon

# TODO: a lot of reads probably need to be changed to signed reads
# eg, room CA sources
# TODO: lots of unknown values :)

identifiers = {}
try:
	for i in open("identifiers.txt").readlines():
		x = i.strip().split("\t")
		identifiers[x[0]] = x[1]
except IOError:
	pass

def getidentifier(family, genus, species):
	x = "%d%02d%03d" % (family, genus, species)
	if x in identifiers:
		return identifiers[x]
	else:
		return None

# utility functions to read values from a stream
import struct

def reads32(f):
	x = f.read(4)
	return struct.unpack("<i", x)[0]

def read32(f):
	x = f.read(4)
	return struct.unpack("<I", x)[0]

def read16(f):
	x = f.read(2)
	return struct.unpack("<H", x)[0]

def read8(f):
	x = f.read(1)
	return struct.unpack("<B", x)[0]

def readstring(f):
	length = read8(f)
	if length == 255:
		length = read16(f)
		if length == 65535:
			length = read32(f)
	return f.read(length)

# read an MFC class from file object f
# if you pass reqclass, the code will assert it got an object of that class
existingobjects = []

def slurpMFC(f, reqclass = None):
	global existingobjects
	# read the pid (this only works up to 0x7ffe, but we'll cope)
	x = read16(f)
	if x == 0:
		# null object
		return None
	elif x == 65535:
		# completely new class
		schemaid = read16(f)
		strlen = read16(f)
		name = f.read(strlen)
		newclass = sys.modules['__main__'].__dict__[name]
		existingobjects.append(newclass)
	elif x & 0x8000 != 0x8000:
		# return an existing object which we already read
		assert x - 1 < len(existingobjects)
		o = existingobjects[x - 1]
		#if reqclass:
		#	assert o.__class__ == reqclass
		#print "* Returning a " + newclass.__name__ + " (id #" + str(x) + ")"
		return o
		#return None
	else:
		# create an instance of an already-existing class
		x = x ^ 0x8000
		newclass = existingobjects[x - 1]

	if reqclass:
		assert newclass == reqclass, "slurpMFC wanted " + reqclass.__name__ + " but got " + newclass.__name__
	#print "* creating: " + newclass.__name__
	n = newclass()
	existingobjects.append(n)
	n.read(f)
	return n

# creatures classes
# this is where all the real logic is - read() slurps up the contents

class CDoor:
	def read(self, f):
		self.openness = read8(f)
		self.otherroom = read16(f)
		x = read16(f)
		assert x == 0, "CDoor zero wasn't zero"

# TODO:
# we don't know where 'visited flag' is...

nextroom = 0
class CRoom:
	def read(self, f):
		# room id
		self.roomid = read32(f)

		# check rooms are in order, as they should be, maybe?
		global nextroom
		assert nextroom == self.roomid
		nextroom = self.roomid + 1

		# read magic constant
		x = read16(f)
		assert x == 2, "magic constant for CRoom wasn't 2"

		# read room bounding box
		self.left = read32(f)
		self.top = read32(f)
		self.right = read32(f)
		self.bottom = read32(f)

		# read doors from four directions
		self.doordirections = []
		for i in range(4):
			nodoors = read16(f)
			self.doordirections.append([])
			for j in range(nodoors):
				x = slurpMFC(f, CDoor)
				assert x
				self.doordirections[i].append(x)

		# read roomtype, 0-3 in C2
		self.roomtype = read32(f)
		assert self.roomtype < 4, "CRoom found weird roomtype: " + str(self.roomtype)

		# slurp up some data
		self.floorvalue = read8(f)
		self.inorganicnutrients = read8(f)
		self.organicnutrients = read8(f)
		self.temperature = read8(f)
		self.heatsource = reads32(f)
		self.pressure = read8(f)
		self.pressuresource = reads32(f)

		# wind x/y direction
		self.windx = reads32(f)
		self.windy = reads32(f)

		# slurp up some more data
		self.lightlevel = read8(f)
		self.lightsource = reads32(f)

		# slurp up some more data
		self.radiation = read8(f)
		self.radiationsource = reads32(f)

		# TODO: slurp up 800 unknown bytes! disease data?
		self.randombytes = f.read(800)

		# read floor points
		nopoints = read16(f)
		assert nopoints < 100, "sanity check on nopoints"
		self.floorpoints = []
		for i in range(nopoints):
			pointx = read32(f)
			pointy = read32(f)
			self.floorpoints.append((pointx, pointy))

		# TODO: slurp up unknown value
		x = read32(f)
		assert x == 0, "first footer of CRoom was " + str(x)

		self.music = readstring(f)
		self.dropstatus = read32(f)
		assert self.dropstatus < 3, "dropstatus was " + str(self.dropstatus)

class CGallery:
	def read(self, f):
		# number of sprites
		self.noframes = read32(f)
		# four chars of filename
		self.filename = f.read(4)
		self.firstsprite = read32(f)
		# TODO: number of references?
		self.refs = read32(f)

		# read each piece of framedata in
		self.framedata = []
		for i in range(self.noframes):
			data = {}
			x = read8(f) # TODO
			# i suspect this may be 'no of references' - fuzzie
			#assert x == 1 or x == 4, "CGallery framedata 1st byte was " + str(x)

			x = read8(f) # TODO
			# apparently this is '6' in the first eden SimpleObject o.O
			#assert x == 0, "CGallery framedata 2nd byte was " + str(x)

			x = read8(f) # TODO
			# apparently this is 4 inside eden obj #370, a blackboard
			# apparently this is 5 inside eden obj #552, a blackboard
			assert x == 0 or x == 1 or x == 4 or x == 5, "CGallery framedata 3rd byte was " + str(x)

			data['width'] = read32(f)
			data['height'] = read32(f)
			data['offset'] = read32(f)

		print "gallery read file '" + self.filename + "' with " + str(self.noframes) + " frame(s) starting at " + str(self.firstsprite)

class MapData:
	def read(self, f):
		global version
		version = read32(f)
		if version == 0:
			print "this is a Creatures 1 SFC file"
		elif version == 1:
			print "this is a Creatures 2 SFC file"
		else:
			assert false, "didn't understand version " + str(version)
		self.savecount = read16(f) # TODO: prbly not actually savecount
		x = read16(f) # <- this is prbly uint32 bit
		assert x == 0, "third bytes of MapData were not zero but " + str(x)
		if version == 1:
			x = read32(f) # TODO who knows? 0 (common) or 4, so far
			assert x == 0 or x == 4, "fourth bytes of MapData were not zero but " + str(x)
			x = read32(f) # TODO
			assert x == 0, "fifth bytes of MapData were not zero but " + str(x)

		# sprite for background tiles
		self.gallery = slurpMFC(f, CGallery)

		# rooms
		self.roomcount = read32(f)
		print "trying to read " + str(self.roomcount) + " rooms"
		self.rooms = []
		for i in xrange(self.roomcount):
			if version == 0:
				left = reads32(f)
				top = read32(f)
				right = reads32(f)
				bottom = read32(f)
				roomtype = read32(f)
				assert roomtype < 3
				roomtypes = ["Indoors", "Surface", "Undersea"]
				print "room at (" + str(left) + ", " + str(top) + "), (" + str(right) + ", " + str(bottom) + ") - type " + roomtypes[roomtype]
				# TODO
				self.rooms.append(None)
			else:
				assert version == 1
				self.rooms.append(slurpMFC(f, CRoom))

		if version == 0:
			# ground level data
			self.groundlevel = []
			for x in xrange(261):
				self.groundlevel.append(read32(f))

			# mysterious random bytes! like the in-room ones of C2
			# disease data? TODO
			self.randombytes = f.read(800)

readingcompound = False
readingscenery = False

class Entity: # like a compound part?
	def read(self, f):
		print "--- start part ---"

		# sprite
		self.sprite = slurpMFC(f)
		print "sprite file: " + self.sprite.filename

		# currently displayed sprite details
		self.currsprite = read8(f)
		self.imageoffset = read8(f)
		print "curr sprite#: " + str(self.currsprite) + ", img offset: " + str(self.imageoffset)
		
		# zorder
		# TODO: not sure if this should be signed, it makes pointer weird
		self.zorder = reads32(f)
		print "zorder: " + str(self.zorder)

		# location
		self.x = read32(f)
		self.y = read32(f)
		print "loc: " + str(self.x) + ", " + str(self.y)

		self.haveanim = read8(f)
		assert self.haveanim == 0 or self.haveanim == 1
		if self.haveanim == 1:
			self.animframe = read8(f)
			if version == 0:
				self.animstring = f.read(32)
			else:
				self.animstring = f.read(99)
			x = self.animstring.find("\0")
			if x != -1:
				self.animstring = self.animstring[:x]
			print "on frame " + str(self.animframe) + " of animation '" + self.animstring + "'"

		if readingscenery:
			return

		if readingcompound:
			self.relx = read32(f)
			self.rely = read32(f)
			print "part offset: " + str(self.relx) + ", " + str(self.rely)
			return

		self.zorder2 = reads32(f)
		if self.zorder != self.zorder2:
			# kaelis's Eden.sfc has this differing for obj#816!
			# TODO: work out what the heck
			print "strange zorder: " + str(self.zorder2)

		# TODO: finish decoding this
		self.clickbhvr = f.read(3)
		self.touchbhvr = read8(f)
		print "* touch bhvr: " + str(self.touchbhvr) + ", click bhvr:",
		for z in self.clickbhvr: print "%02X" % ord(z),
		print

		if version == 0: return
		
		num_pickup_handles = read16(f)
		self.pickup_handles = []
		for i in xrange(num_pickup_handles):
			self.pickup_handles.append((read32(f), read32(f)))

		num_pickup_points = read16(f)
		self.pickup_points = []
		for i in xrange(num_pickup_points):
			self.pickup_points.append((read32(f), read32(f)))

		print "read " + str(len(self.pickup_handles)) + " pickup handles and " + str(len(self.pickup_points)) + " pickup points"

class Object:
	def partialread(self, f):
		if version == 0:
			# TODO
			# we should make sure this value doesn't actually matter.
			# for now fuzzie is assuming it doesn't, it's presumably the unused portion
			# of CLAS - it's not always zero, some Terra Nornia scenery has it as 0xff
			x = read8(f)
			#assert x == 0, "Object lacking nulls at start, instead has " + str(x)
		# genus/family/species
		if version == 0:
			self.species = read8(f)
			self.genus = read8(f)
			self.family = read8(f)
		else:
			self.genus = read8(f)
			self.family = read8(f)
			x = read16(f)
			assert x == 0, "Object lacking nulls at start, instead has " + str(x)
			self.species = read16(f)

		# print nice stuff!
		identifier = getidentifier(self.family, self.genus, self.species)
		if not identifier:
			identifier = ""
		else:
			identifier = " - '" + identifier + "'"
		print "agent " + self.__class__.__name__ + ": (" + str(self.family) + ", " + str(self.genus) + ", " + str(self.species) + ")" + identifier + ","

		if version == 0:
			x = read8(f)
			# TODO: should make sure this is really only set for pointer?
			# might be mysteriousness byte from c2..
			if x: print "pointer-only byte was" + str(x)
		else:
			# unid
			self.unid = read32(f)
			print "unid: " + str(self.unid)

			# TODO
			x = read8(f)
			# TODO: 0 or 1 normally, but 'Hook', unid: 56314, at least, has it at 4
			assert x == 0 or x == 1 or x == 4
			print "* mysteriousness (0/1/4): " + str(x)

		# attributes
		if version == 0:
			# TODO: verify this sometime :)
			self.attr = read8(f)
		else:
			self.attr = read16(f)
		print "attr: " + str(self.attr)

		if version == 1:
			zarros = read16(f)
			assert zarros == 0

		one = read32(f)
		two = read32(f)
		three = read32(f)
		four = read32(f)

		zarros = read16(f)
		if version == 0:
			assert zarros == 0, "zarros: " + str(zarros)
		else:
			# drat, PointerTool in eden has this as 1803
			if zarros != 0:
				print "zarros: " + str(zarros)

		self.actv = read8(f)
		print "coords? " + str(one) + ", " + str(two) + ", " + str(three) + ", " + str(four) + ", actv: " + str(self.actv)

		# our sprite
		self.sprite = slurpMFC(f, CGallery)

		# tick data
		self.tickreset = read32(f)
		self.tickstate = read32(f)
		assert self.tickreset >= self.tickstate
		print "* tick time: " + str(self.tickreset) + ", state: " + str(self.tickstate)

		x = read16(f)
		assert x == 0
		self.currentsound = f.read(4)
		if self.currentsound[0] != chr(0):
			print "current sound: " + self.currentsound

		if version == 0: numvariables = 3
		else: numvariables = 100

		# OBVx variables
		self.variables = []
		for i in xrange(numvariables):
			self.variables.append(read32(f))
		
		if version == 1:
			# misc physics-ish data
			self.size = read8(f)
			self.range = read32(f)
			x = read32(f) # TODO: FFFF FFFF?
			if x != 0xffffffff:
				print "* mysterious physicsish value: " + str(x)
			self.accg = read32(f)
			self.velx = reads32(f)
			self.vely = reads32(f)
			print "velx: " + str(self.velx) + ", vely: " + str(self.vely)
			self.rest = read32(f)
			self.aero = read32(f)
			x = f.read(6) # TODO: unknown [for pointer: 0000 0400 0000]
					# [for eden #1: d101 0400 0000]
			print "* post-physics bytes:",
			for z in x: print "%02X" % ord(z),
			print

			self.threat = read8(f)

			print "accg: " + str(self.accg) + ", rest: " + str(self.rest) + ", aero: " + str(self.aero) + ", size: " + str(self.size) + ", range: " + str(self.range) + ", threat: " + str(self.threat)
		
			# TODO: 01 normally, 03 when frozen, 00 for scenery?
			self.flags = read8(f)
			assert self.flags == 0 or self.flags == 1 or self.flags == 3, str(self.flags)

		# TODO: sane scriptness
		self.scripts = {}
		numscripts = read32(f)
		for i in range(numscripts):
			if version == 0:
				eventno = read8(f)
				species = read8(f)
				genus = read8(f)
				family = read8(f)
			else:
				genus = read8(f)
				family = read8(f)
				eventno = read16(f)
				species = read16(f)
			script = readstring(f)
			print "event #" + str(eventno) + " for " + str(family) + ", " + str(genus) + ", " + str(species) + ": " + script
			self.scripts[eventno] = script

class SimpleObject(Object):
	def read(self, f):
		Object.partialread(self, f)

		self.entity = slurpMFC(f, Entity)
		assert self.entity

class PointerTool(SimpleObject):
	def read(self, f):
		SimpleObject.read(self, f)

		# TODO: data from C2 eden is shown below
		# 02 00 00 00 02 00 00 00
		# 00 00 00 00 00 00
		# 67 65 74 00
		# 63 72 69 74
		# 74 65 72 00
		# 66 67 00 00
		# 00 00 00 00 00 00 00 00 00
		# CD CD CD CD CD CD CD CD CD CD CD CD
		# bear in mind CD CD CD CD is prbly unallocated memory from microsoft's CRT
		if version == 0:
			x = f.read(35)
		else:
			x = f.read(8 + 6 + 16 + 9 + 12)
		print "pointer bytes: ",
		for z in x: print "%02X" % ord(z),
		print

class CompoundObject(Object):
	def read(self, f):
		Object.partialread(self, f)

		global readingcompound
		readingcompound = True

		num_parts = read32(f)
		self.parts = []
		print "reading " + str(num_parts) + " parts.."
		for i in xrange(num_parts):
			print "reading part #" + str(i)
			e = slurpMFC(f, Entity)
			if not e:
				# TODO: hackery?
				x = f.read(8)
				print "part bytes:",
				for z in x: print "%02X" % ord(z),
				print
			if i == 0:
				assert e, "part 0 was null"
				assert e.relx == 0 and e.rely == 0
			self.parts.append(e)
		readingcompound = False

		self.hotspots = []
		for i in range(6):
			hotspotinfo = {}
			hotspotinfo['left'] = reads32(f)
			hotspotinfo['top'] = reads32(f)
			hotspotinfo['right'] = reads32(f)
			hotspotinfo['bottom'] = reads32(f)
			self.hotspots.append(hotspotinfo)

		for i in range(6):
			self.hotspots[i]['function'] = reads32(f)
			
		if version == 1:
			for i in range(6):
				self.hotspots[i]['message'] = read16(f)
				self.hotspots[i]['zero'] = read16(f)
				assert self.hotspots[i]['zero'] == 0
			for i in range(6):
				self.hotspots[i]['mask'] = read8(f)
		print self.hotspots

class Vehicle(CompoundObject):
	def read(self, f):
		CompoundObject.read(self, f)

		# TODO: correct?
		self.xvec = reads32(f)
		self.yvec = reads32(f)
		self.bump = read8(f)
		print "xvec: " + str(self.xvec) + ", yvec: " + str(self.yvec) + ", bump flags: " + str(self.bump)

		self.coordx = read16(f)
		if version == 0:
			x = f.read(2)
			print "vehicl bytes:",
			for z in x: print "%02X" % ord(z),
			print
		else:
			x = read16(f)
			assert x == 0
		self.coordy = read16(f)
		x = read8(f)
		assert x == 0
		print "vehicle coords: " + str(self.coordx) + ", " + str(self.coordy)

		# TODO: this could all be nonsense, really
		self.cabinleft = read32(f)
		self.cabintop = read32(f)
		self.cabinright = read32(f)
		self.cabinbottom = read32(f)

		print "cabin coords: " + str(self.cabinleft) + ", " + str(self.cabintop) + ", " + str(self.cabinright) + ", " + str(self.cabinbottom)

		x = read32(f)
		assert x == 0

# TODO: is it Vehicle subclass?
class Lift(Vehicle):
	def read(self, f):
		Vehicle.read(self, f)

#		if version == 0:
#			x = f.read() # TODO
		self.numcallbuttons = read32(f)
		self.curcallbutton = read32(f)
		self.mysterybytes = f.read(5)
		assert self.mysterybytes == "\xff\xff\xff\xff\x00"
		self.callbuttonys = []
		for i in range(8):
			self.callbuttonys.append(read32(f))
			assert read16(f) == 0
		print "call buttons defined: " + str(self.numcallbuttons)
		print "current floor: " + str(self.curcallbutton)
		print "call button y-vals: " + str(self.callbuttonys)
		if version == 1:
			x = f.read(4) # TODO
			print "lift bytes: ",
			for z in x: print "%02X" % ord(z),
			print

class CallButton(SimpleObject):
	def read(self, f):
		SimpleObject.read(self, f)

		self.lift = slurpMFC(f, Lift)
		assert self.lift
		self.buttonid = read8(f)
		print "button #" + str(self.buttonid) + " for lift " + str(self.lift)

class Blackboard(CompoundObject):
	def read(self, f):
		CompoundObject.read(self, f)

		# TODO: none of this is verified
		if version == 0:
			self.textx = read8(f)
			self.texty = read8(f)
			self.backgroundcolour = read8(f)
			self.chalkcolour = read8(f)
			self.aliascolour = read8(f)
		else:
			self.textx = read32(f)
			self.texty = read32(f)
			self.backgroundcolour = read16(f)
			self.chalkcolour = read16(f)
			self.aliascolour = read16(f)
		print "text at (" + str(self.textx) + ", " + str(self.texty) + ")"
		print "colours: " + str(self.backgroundcolour) + ", " + str(self.chalkcolour) + ", " + str(self.aliascolour)

		if version == 0:
			totalwords = 16
		else:
			totalwords = 48

		for i in xrange(totalwords):
			num = read32(f)
			x = f.read(11)
			d = x.find("\0")
			if d != -1:
				x = x[:d]
			if len(x) > 0:
				print "blackboard string #" + str(num) + ": " + x

class Scenery(SimpleObject):
	def read(self, f):
		global readingscenery
		readingscenery = True
		SimpleObject.read(self, f)
		readingscenery = False

# -------------------------------------------------------------------

import sys

assert len(sys.argv) == 2
f = open(sys.argv[1], "r")

print "reading map data.."

# slurp up the map data
data = slurpMFC(f, MapData)

print "successfully read " + str(len(data.rooms)) + " rooms"

# seek through nulls to find the number of objects
x = 0
while x == 0:
	x = read8(f)
f.seek(-1, 1)

# read number of objects
objects = []
numobjects = read32(f)
print "reading " + str(numobjects) + " objects.."
print

# read all the objects
for i in range(numobjects):
	print "object #" + str(i + 1) + ":"
	x = slurpMFC(f)
	if x == None:
		print "made a mistake somewhere :("
		sys.exit(0)
	print
	objects.append(x)

scenery = []
numscenery = read32(f)
print "reading " + str(numscenery) + " scenery objects.."
print

# read all the scenery objects
for i in range(numscenery):
	print "scenery object #" + str(i + 1) + ":"
	x = slurpMFC(f, Scenery)
	if x == None:
		print "made a mistake somewhere :("
		sys.exit(0)
	print
	scenery.append(x)

# TODO: sane scriptness
scripts = {}
numscripts = read32(f)
print "reading " + str(numscripts) + " scripts.."
for i in range(numscripts):
	if version == 0:
		eventno = read8(f)
		species = read8(f)
		genus = read8(f)
		family = read8(f)
	else:
		genus = read8(f)
		family = read8(f)
		eventno = read16(f)
		species = read16(f)
	script = readstring(f)
	print "event #" + str(eventno) + " for " + str(family) + ", " + str(genus) + ", " + str(species) + " (global): " + script
	scripts[eventno] = script

print

# TODO: not sure about these, but they change when i scroll :)
scrollx = read32(f)
scrolly = read32(f)
print "scrolled to " + str(scrollx) + ", " + str(scrolly)
# TODO: definitely not sure about these!
zeros = read16(f)
assert zeros == 0
favplacename = readstring(f)
print "aaaand, to finish off, our favourite place is: " + favplacename

if version == 0:
	sys.exit(0)

print
print "and now, for " + str(len(data.rooms)) + " rooms.."
print

roomtypes = ["In-Doors", "Surface", "Underwater", "Atmosphere"]
dropstatuses = ["Never", "Above-floor", "Always"]
doordirs = ["Left", "Right", "Up", "Down"]

for i in data.rooms:
	print "room # " + str(i.roomid) + " at (" + str(i.left) + ", " + str(i.top) + "), to (" + str(i.right) + ", " + str(i.bottom) + ")"
	for j in range(4):
		print "doors in direction " + doordirs[j] + ":",
		if len(i.doordirections[j]) == 0:
			print "None.",
		for k in i.doordirections[j]:
			if k != i.doordirections[j][0]: print ",",
			print "openness " + str(k.openness) + " to room #" + str(k.otherroom),
		print
	print "wind: (" + str(i.windx) + ", " + str(i.windy) + ")"
	print "room type: " + roomtypes[i.roomtype] + " (" + str(i.roomtype) + ")"
	print "floor value: " + str(i.floorvalue)
	print "inorganic nutrients: " + str(i.inorganicnutrients) + ", organic nutrients: " + str(i.organicnutrients)
	print "temperature: " + str(i.temperature) + ", heat source: " + str(i.heatsource)
	print "pressure: " + str(i.pressure) + ", pressure source: " + str(i.pressuresource)
	print "radiation: " + str(i.radiation) + ", radiation source: " + str(i.radiationsource)
	if len(i.music) > 0:
		print "music: " + str(i.music)
	if len(i.floorpoints) > 0:
		print "Surface points:",
		for x in i.floorpoints:
			print "(" + str(x[0]) + ", " + str(x[1]) + ")",
		print
	print "drop status: " + dropstatuses[i.dropstatus] + " (" + str(i.dropstatus) + ")"
	print

