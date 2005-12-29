#ifndef CMDDATA_H
#define CMDDATA_H 1

class caosVM;

struct cmdinfo {
	int idx;
	const char *key;
	const char *variant;
	const char *name;
	const char *fullname;
	const char *docs;
	int argc;
	int retc;
	void (caosVM::*handler)();
	// TODO: arg types
};

void registerAutoDelegates();

#endif


/* vim: set noet: */
