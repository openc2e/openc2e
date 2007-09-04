/*
 *  token.h
 *  openc2e
 *
 *  Created by Bryan Donlan on Thu 11 Aug 2005.
 *  Copyright (c) 2005 Bryan Donlan. All rights reserved.
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
#ifndef TOKEN_H
#define TOKEN_H 1

#include "openc2e.h"
#include "caosVar.h"
#include "exceptions.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <boost/variant.hpp>
#include <boost/format.hpp>

void yyrestart(std::istream *stream, bool use_c2);

enum toktype { ANYTOKEN = 0, EOI = 0, TOK_WHITESPACE = 0, TOK_CONST, TOK_WORD, TOK_BYTESTR, TOK_NEWLINE };


struct token {
	struct token_eoi { };
	struct token_nl { };
	struct type_visitor : public boost::static_visitor<toktype> {
		toktype operator()(const token_eoi) const { return EOI; }
		toktype operator()(const token_nl) const { return TOK_NEWLINE; }
		toktype operator()(const caosVar &) const { return TOK_CONST; }
		toktype operator()(const std::string &) const { return TOK_WORD; }
		toktype operator()(const bytestring_t &) const { return TOK_BYTESTR; }
	};
	struct dump_visitor : public boost::static_visitor<std::string> {
		std::string operator()(const token_eoi) const { return std::string("EOI"); }
		std::string operator()(const token_nl) const { return std::string("NL"); }
		std::string operator()(const caosVar &v) const { return std::string(v.dump()); }
		std::string operator()(const std::string &v) const { return v; }
		std::string operator()(const bytestring_t &bs) const {
			std::ostringstream oss;
			oss << "[ ";
			for (size_t i = 0; i < bs.size(); i++)
				oss << bs[i] << " ";
			oss << "]";
			return oss.str();
		}
	};
	struct fmt_visitor : boost::static_visitor<std::string> {
		std::string operator()(const token_eoi) const { return std::string("<EOI>"); }
		std::string operator()(const token_nl) const { return std::string("\n"); }
		std::string operator()(const caosVar &v) const {
			if (v.hasInt()) {
				return boost::str(boost::format("%d") % v.getInt());
			} else if (v.hasFloat()) {
				return boost::str(boost::format("%f") % v.getInt());
			} else if (v.hasString()) {
				std::ostringstream outbuf;
				std::string inbuf = v.getString();
				outbuf << '"';
				for (size_t i = 0; i < inbuf.size(); i++) {
					switch (inbuf[i]) {
						case '\n': outbuf << "\\\n"; break;
						case '\r': outbuf << "\\\r"; break;
						case '\t': outbuf << "\\\t"; break;
						case '\"': outbuf << "\\\""; break;
						default:   outbuf << inbuf[i]; break;
					}
				}
				outbuf << '"';
				return outbuf.str();
			} else {
				throw creaturesException(std::string("Impossible caosvar type in token: ") + v.dump());
			}
		}
		std::string operator()(const std::string &word) const { return word; }
		std::string operator()(const bytestring_t &bs) const {
			std::ostringstream oss;
			oss << "[ ";
			for (size_t i = 0; i < bs.size(); i++)
				oss << i << " ";
			oss << "]";
			return oss.str();
		}
	};

				


	boost::variant<token_eoi, token_nl, std::string, caosVar, bytestring_t> payload;
	template<class T> token(const T &copyFrom) { payload = copyFrom; }

	int index;
	int lineno;

	toktype type() const {
		return boost::apply_visitor(type_visitor(), payload);
	}

	const bytestring_t &bytestr() const {
		const bytestring_t *bs = boost::get<bytestring_t>(&payload);
		if (!bs) unexpected();
		return *bs;
	}

	const std::string &word() const {
		const std::string *s = boost::get<std::string>(&payload);
		if (!s) unexpected();
		return *s;
	}

	const caosVar &constval() const {
		const caosVar *s = boost::get<caosVar>(&payload);
		if (!s) unexpected();
		return *s;
	}
	token() {}
	token(const token &cp) : payload(cp.payload), index(cp.index), lineno(cp.lineno) { }

	std::string dump() const {
		std::ostringstream oss;
		oss << boost::apply_visitor(dump_visitor(), payload);
		oss << " (line " << lineno << ")";
		return oss.str();
	}

	std::string format() const {
		return boost::apply_visitor(fmt_visitor(), payload);
	}

	void unexpected() const {
		const char *err;
		switch (type()) {
			case EOI: err = "EOI"; break;
			case TOK_WORD: err = "word"; break;
			case TOK_BYTESTR: err = "bytestring"; break;
			case TOK_CONST: err = "constant value"; break;
			default: throw creaturesException("Internal error: unable to classify token in token::unexpected()");
		}

		throw parseException(std::string("Unexpected ") + err);
	}
};

#endif

/* vim: set noet: */
