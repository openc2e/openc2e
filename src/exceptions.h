/*
 *  exceptions.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sat 13 Nov 2004.
 *  Copyright (c) 2004-2005 Alyssa Milburn. All rights reserved.
 *  Copyright (c) 2005-2006 Bryan Donlan. All rights reserved.
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
#ifndef __C2EEXCEPTIONS_H
#define __C2EEXCEPTIONS_H

#include <exception>
#include <cstdlib>
#include <string>
#include <string.h>
#include <vector>
#include <sstream>
#include <assert.h>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <iostream> // for stupid copy constructor debug below

class script;

class creaturesException : public std::exception {
protected:
	bool malloced;
	const char *r;


public:
	virtual std::string prettyPrint() const { return std::string(what()); }
	creaturesException(const creaturesException &e) throw() : std::exception() {
		// catch clause missing & etc, we'll try to patch over it but you should fix it really.
		std::cerr << "QA: creaturesException copy constructor called." << std::endl;
		r = strdup(e.r);
		malloced = true;
	}
	creaturesException(const char *s) throw() { r = s; malloced = false; }
	creaturesException(const std::string &s) throw() {
		r = strdup(s.c_str());
		if (!r) {
			abort();
		}
		malloced = true;
	}
	~creaturesException() throw() {
		if (malloced) free((void *)r);
	}
	const char* what() const throw() { return r; }
};

class caosException : public creaturesException {
	protected:
		boost::shared_ptr<class script> script;
		int traceindex;

	public:
		/* debug hook, removeme */
		virtual const char *what() const throw() { return this->creaturesException::what(); }

		~caosException() throw() { }
		
		caosException(const std::string &d) throw() : creaturesException(d), traceindex(-1) { }

		caosException(const char *d) throw() : creaturesException(d), traceindex(-1) { }

		void trace(boost::shared_ptr<class script> scr, int traceindex = -1) throw();

		virtual std::string prettyPrint() const;
};
		
		

static inline std::string buildExceptionString(const char *s, const char *file, int line) {
	std::ostringstream oss;
	oss << s << " at " << file << ':' << line;
	return oss.str();
}
	

struct tracedAssertFailure : public creaturesException {
	tracedAssertFailure(const char *s, const char *file, int line)
	: creaturesException(buildExceptionString(s, file, line)) { }
};
		

class tokeniseFailure : public creaturesException {
public:
	tokeniseFailure(const char *s) throw() : creaturesException(s) { }
	tokeniseFailure(const std::string &s) throw() : creaturesException(s) { }
};

class parseFailure : public creaturesException {
public:
	parseFailure(const char *s) throw()
		: creaturesException(s), lineno(-1) { }
	parseFailure(const std::string &s) throw()
		: creaturesException(s), lineno(-1) { }
	~parseFailure() throw() { }

	boost::shared_ptr<std::vector<class token> > context;
	int ctxoffset;
	std::string filename;
	int lineno;

	std::string prettyPrint() const;
};

typedef parseFailure parseException;

class genomeException : public creaturesException {
public:
	genomeException(const char *s) throw() : creaturesException(s) { }
	genomeException(const std::string &s) throw() : creaturesException(s) { }
};

#endif
/* vim: set noet: */
