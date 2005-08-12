#ifndef __C2EEXCEPTIONS_H
#define __C2EEXCEPTIONS_H

#include <exception>
#include <cstdlib>
#include <string>
#include <sstream>
#include <assert.h>

class creaturesException : public std::exception {
protected:
	bool malloced;
	const char *r;

public:
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

class caosException : public std::exception {
	protected:
		bool desc_is_mine;
		bool fn_is_mine;
		char *desc;
		char *filename;
		int line;
		class caosOp *op;

		mutable const char *cache;
		
		const char *cacheDesc() const throw() {
			if (cache)
				return cache;
			if (filename) {
				std::ostringstream oss;
				oss << desc;
				oss << " at " << filename;
				if (line != -1)
					oss << ":" << line;
				cache = strdup(oss.str().c_str());
			} else {
				cache = desc;
			}
			return cache;
		}
	public:

		~caosException() throw() {
			if (cache && cache != desc)
				free(const_cast<char *>(cache));
			if (fn_is_mine)
				free(filename); // free(NULL) is safe
			if (desc_is_mine)
				free(desc);
		}
		
		caosException(const std::string &d) throw() {
			desc_is_mine = true;
			fn_is_mine = false;
			desc = strdup(d.c_str());
			if (!desc) abort();
			filename = NULL;
			line = -1;
			op = NULL;
			cache = NULL;
		}

		caosException(const char *d) throw() {
			desc_is_mine = false;
			fn_is_mine = false;
			desc = const_cast<char *>(d);
			assert(desc);
			filename = NULL;
			line = -1;
			op = NULL;
			cache = NULL;
		}

		const char *what() const throw() {
			return cacheDesc();
		}

		void trace(const char *filename, int line, caosOp *op) {
			this->filename = strdup(filename);
			fn_is_mine = true;
			this->line = line;
			this->op = op;
			if (cache && cache != desc)
				free(const_cast<char *>(cache));
			cache = NULL;
		}
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
	parseFailure(const char *s) throw() : creaturesException(s) { }
	parseFailure(const std::string &s) throw() : creaturesException(s) { }
};

typedef parseFailure parseException;

class genomeException : public creaturesException {
public:
	genomeException(const char *s) throw() : creaturesException(s) { }
	genomeException(const std::string &s) throw() : creaturesException(s) { }
};

#endif
/* vim: set noet: */
