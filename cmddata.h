#ifndef CMDDATA_H
#define CMDDATA_H 1

struct cmdinfo {
	const char *name;
	const char *fullname;
	const char *docs;
	int argc;
	int retc;
	// TODO: arg types
};

extern const cmdinfo cmds[];
void registerAutoDelegates();

#endif


/* vim: set noet: */
