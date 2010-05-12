#!/usr/bin/python

# this implements the algorithm for using a .vce file to convert a piece of
# text into creature sound via splitting into syllables and using a lookup
# table to find the appropriate sound file entry

# it's stored here just for reference, until someone implements it in C++..
# it is quite possibly full of bugs. feel free to fix.

import os
import sys
import struct
import time

def read32(f):
	x = f.read(4)
	return struct.unpack("<I", x)[0]

def readstring(f):
	x = f.read(4)
	if x[0] == chr(0): return ""
	return x

VoiceFiles = []
VoiceLookup = []

class VoiceProcessor:
	def __init__(self, speech):
		assert len(VoiceFiles) == 32
		assert len(VoiceLookup) == 81

		speech = speech.lower() # we work in lowercase
		self.speech = "." # start/end with dots
		for i in speech:
			# ignore everything which isn't a space or a-z
			if i == ' ':
				if len(self.speech) > 1 and self.speech[-1] != ' ':
					self.speech = self.speech + i
			elif i >= 'a' and i <= 'z':
				self.speech = self.speech + i
		self.speech = self.speech + "."
		self.offset = 1 # offset into self.speech

	def transform(self, chars):
		# magic lookup table transform!
		data = VoiceLookup[chars[0]] & (VoiceLookup[chars[1] + 27] & VoiceLookup[chars[2] + 27 + 27])
		if chars[1] == 26:
			data = data & 0xf # bits 0-3
		if chars[0] == 26:
			data = data & 0x7f0 # bits 4-10
		if chars[2] == 26:
			data = data & 0x3f800 # bits 11-17
		if chars[0] < 26 and chars[1] < 26 and chars[2] < 26:
			data = data & 0xfffc0000 # bits 18-31
		if data == 0:
			# eep! caller will handle it
			return 32

		ourlist = [] # empty array
		for i in xrange(32): # for each of 32 bits
			if data & 1 == 1:
				ourlist.append(i)
			data = data >> 1
		data = chars[0] + chars[1] + chars[2]
		data = data % len(ourlist)
		return ourlist[data]

	def getsound(self, chars):
		ourchars = []
		for i in chars:
			if i == ' ' or i == '.':
				ourchars.append(26)
			else:
				ourchars.append(ord(i) - ord('a'))
		transformed = self.transform(ourchars)

		# out of range (insufficient data for a syllable?)
		if transformed >= 32 or transformed == 0:
			transformed = ourchars[0] + ourchars[1] + ourchars[2]
			# transform into 0-3, 4-10, 11-17 or 18-31 ranges, as with the lookup table version
			if ourchars[1] == 26:
				transformed = (transformed % 4)
			elif ourchars[0] == 26:
				transformed = (transformed % 7) + 4
			elif ourchars[2] == 26:
				transformed = (transformed % 7) + 10 # originally 10?
			else:
				transformed = (transformed % 14) + 18

		return VoiceFiles[transformed]

	def getsyllable(self):
		if (self.offset + 1 >= len(self.speech)): return None
		chars = [self.speech[self.offset-1], self.speech[self.offset], self.speech[self.offset+1]]
		x = self.getsound(chars)
		self.offset += 1 # onward!
		if (self.offset + 1 < len(self.speech)):
			chars = [self.speech[self.offset-1], self.speech[self.offset], self.speech[self.offset+1]]
			if chars[0] >= 'a' and chars[0] <= 'z':
				if chars[1] >= 'a' and chars[1] <= 'z':
					if chars[2] >= 'a' and chars[2] <= 'z':
						# we're in the middle of a word, move onward some more
						self.offset += 1
		if x:
			return x
		else:
			return self.getsyllable()

if len(sys.argv) != 3:
	print "syntax: translate_voice.py /path/to/file.vce speech"
	sys.exit(1)

filename = sys.argv[1]
speech = sys.argv[2]

f = open(filename, "r")

for i in xrange(32):
	VoiceFiles.append([readstring(f), read32(f)])

for i in xrange(3):
	for j in xrange(27):
		VoiceLookup.append(read32(f))

# just some silliness for testing, no idea if the sleep is the right idea..
processor = VoiceProcessor(speech)
while True:
	x = processor.getsyllable()
	if not x: break
	print "'" + x[0] + "' (" + str(x[1]) + ")",
	os.popen2("mplayer -ao alsa ~/creatures2/Sounds/" + x[0].lower() + ".wav > /dev/null 2> /dev/null")
	time.sleep(x[1] / 10.0)

