#ifndef CMDDATA_H
#define CMDDATA_H 1

enum ci_types {
	CI_OTHER = -1,
	CI_END = 0,
	CI_NUMERIC,
	CI_STRING,
	CI_AGENT,
	CI_VARIABLE,
	CI_BYTESTR
};

struct cmdinfo {
	const char *name;
	const char *fullname;
	const char *docs;
	int argc;
	int retc;
	const enum ci_types *argtypes;
};

void registerAutoDelegates();

#endif


/* vim: set noet: */
