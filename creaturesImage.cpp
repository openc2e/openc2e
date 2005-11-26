/*
 *  creaturesImage.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sun Jun 06 2004.
 *  Copyright (c) 2004 Alyssa Milburn. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 */

#include "creaturesImage.h"
#include "c16Image.h"
#include "blkImage.h"
#include "openc2e.h"
#include "fileSwapper.h"
#include "PathResolver.h"
#include <iostream>
#include <fstream>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <sys/types.h> // passwd*
#include <pwd.h> // getpwuid

using namespace boost::filesystem;

imageGallery gallery;

enum filetype { blk, s16, c16 };

path cacheDirectory() {
	path p;
	char *envhome = getenv("HOME");
	if (envhome)
		p = path(envhome, native);
	if ((!envhome) || (!is_directory(p)))
		p = path(getpwuid(getuid())->pw_dir, native);
	if (!is_directory(p)) {
		std::cerr << "Can't work out what your home directory is, giving up and using /tmp for now." << std::endl;
		p = path("/tmp", native); // sigh
	}
	p = path(p.native_directory_string() + "/.openc2e", native);
	if (!exists(p))
		create_directory(p);
	else if (!is_directory(p))
		throw creaturesException("Your .openc2e is a file, not a directory. That's bad.");
	return p;
}

bool tryOpen(mmapifstream *in, std::string fname, filetype ft) {
	path cachefile, realfile;
	std::string cachename;

	// work out where the real file should be
	switch (ft) {
		case blk:
			realfile = path(datapath + "/Backgrounds/" + fname, native); break;

		case c16:
		case s16:
			realfile = path(datapath + "/Images/" + fname, native); break;
	}

	std::cout << "looking for " << realfile.native_file_string() << std::endl;

	// if it doesn't exist, too bad, give up.
	if (!resolveFile(realfile)) return false;
	
	// work out where the cached file should be
	cachename = cacheDirectory().native_directory_string() + "/" + fname;
#ifdef __C2E_BIGENDIAN
	cachename = cachename + ".big";
#endif
	cachefile = path(cachename, native);

	if (resolveFile(cachefile)) {
		// TODO: check for up-to-date-ness
		in->clear();
		in->mmapopen(cachefile.native_file_string());
		return in->is_open();
	}

	in->clear();
	in->mmapopen(realfile.native_file_string());
#ifdef __C2E_BIGENDIAN
	if (in->is_open()) {
		fileSwapper f;
		switch (ft) {
			case blk:
				f.convertblk(realfile.native_file_string(), cachefile.native_file_string());
				break;
			case s16:
				f.converts16(realfile.native_file_string(), cachefile.native_file_string());
				break;
			default:
				return true; // c16 hack
		}
		in->close(); // TODO: close the mmap too! how?
		if (!exists(cachefile)) return false; // TODO: exception?
		in->mmapopen(cachefile.native_file_string());
	}
#endif
	return in->is_open();
}

/*
 * Retrieve an image for rendering use. To retrieve a sprite, pass the name without
 * extension. To retrieve a background, pass the full filename (ie, with .blk).
 */
creaturesImage *imageGallery::getImage(std::string name) {
	// step one: see if the image is already in the gallery
	std::map<std::string, creaturesImage *>::iterator i = gallery.find(name);
	if (i != gallery.end()) {
		creaturesImage *img = i->second;
		img->addRef();
		return img;
	}

	// step two: try opening it in .c16 form first, then try .s16 form
	mmapifstream *in = new mmapifstream();

	if (!tryOpen(in, name + ".s16", s16)) {
		if (!tryOpen(in, name + ".c16", c16)) {
			bool lasttry = tryOpen(in, name, blk);
			if (!lasttry) {
				std::cerr << "imageGallery couldn't find the sprite '" << name << "'" << std::endl;
				return 0;
			}
			gallery[name] = new blkImage(in);
		} else {
			gallery[name] = new c16Image(in);
		}
	} else {
		gallery[name] = new s16Image(in);
	}
	
	in->close(); // doesn't close the mmap, which we still need :)

	return gallery[name];
}

void imageGallery::delImage(creaturesImage *in) {
	in->delRef();
	if (in->refCount() == 0) {
		delete in;
		for (std::map<std::string, creaturesImage *>::iterator i = gallery.begin(); i != gallery.end(); i++) {
			if (i->second == in) { gallery.erase(i); return; }
		}
		std::cerr << "imageGallery warning: delImage got a newly unreferenced image but it isn't in the gallery\n";
	}
}

/* vim: set noet: */
