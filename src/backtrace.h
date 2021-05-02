#pragma once

#ifndef _WIN32

#include <dlfcn.h>
#include <execinfo.h>
#include <inttypes.h>
#include <signal.h>
#include <stdio.h>

#ifdef __GNUG__
#include <cxxabi.h>
#endif

static void signalhandler(int) {
	void* callstack[128];
	int num_frames = backtrace(callstack, 128);
	char** symbols = backtrace_symbols(callstack, num_frames);
	if (!symbols) {
		fprintf(stderr, "backtrace_symbols() returned NULL\n");
		backtrace_symbols_fd(callstack, num_frames, STDERR_FILENO);
		exit(1);
	}
	for (int i = 0; i < num_frames; i++) {
		Dl_info info;
		memset(&info, 0, sizeof(info));
		dladdr(callstack[i], &info);

		// frame number
		fprintf(stderr, "%-3d ", i);

		// filename
		if (info.dli_fname) {
			const char* basename = strrchr(info.dli_fname, '/');
			if (basename) {
				basename++;
			} else {
				basename = info.dli_fname;
			}
			fprintf(stderr, "%-35s ", basename);
		} else {
			fprintf(stderr, "%-35s ", "???");
		}

		// memory address
		fprintf(stderr, "0x%.*" PRIxPTR " ", (int)(sizeof(void*) * 2), (uintptr_t)callstack[i]);

		// function name
		if (info.dli_sname) {
#ifdef __GNUG__
			int status;
			char* demangled = abi::__cxa_demangle(info.dli_sname, NULL, NULL, &status);
			if (status == 0) {
				fprintf(stderr, "%s ", demangled);
				free(demangled);
			} else {
				fprintf(stderr, "%s ", info.dli_sname);
			}
#else
			fprintf(stderr, "%s ", info.dli_sname);
#endif
		} else {
			fprintf(stderr, "0x0 ");
		}

		// offset from function start
		fprintf(stderr, "+ %" PRIuPTR, (uintptr_t)callstack[i] - (uintptr_t)info.dli_saddr);

		// end of frame
		fprintf(stderr, "\n");
	}
	free(symbols);
	exit(1);
}

#endif

void install_backtrace_printer() {
#ifndef _WIN32
	signal(SIGSEGV, signalhandler);
	signal(SIGABRT, signalhandler);
#endif
}