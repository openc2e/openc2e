#!/usr/bin/python

import zlib
import struct
import sys

if len(sys.argv) != 2:
	print "Usage: new_breed_installer_extract.py file.exe"
	sys.exit(1)

inp = open(sys.argv[1])
cdata = inp.read()
if cdata[-4:] !=  "dbrf":
	print "This is not a file created using Kinnison's New Breed Installer, or else it is corrupt."
	sys.exit(1)

datapos = struct.unpack("<I", cdata[-8:-4])[0]
data = zlib.decompress(cdata[datapos:])

currpos = 0

def readstr():
	global currpos
	size = struct.unpack("<I", data[currpos:currpos+4])[0]
	currpos = currpos + 4
	s = data[currpos:currpos + size]
	currpos = currpos + size
	return s

title = readstr()
print "Title: " + title
author = readstr()
print "Author: " + author
desc = readstr()
print "Description/Notes: " + desc

nofiles = struct.unpack("<I", data[currpos:currpos+4])[0]
currpos = currpos + 4

for i in range(nofiles):
	filename = readstr()
	out = open(filename, "w")
	out.write(readstr())

