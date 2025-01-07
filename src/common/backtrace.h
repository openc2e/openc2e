#pragma once

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
// #include <psapi.h>
#include <dbghelp.h>
#include <string.h>
#pragma comment(lib, "dbghelp.lib")

LONG WINAPI TopLevelExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo) {
	HANDLE process = GetCurrentProcess();
	SymInitialize(process, NULL, TRUE);
	DWORD sym_options = SymGetOptions();
	sym_options |= SYMOPT_LOAD_LINES | SYMOPT_UNDNAME;
	SymSetOptions(sym_options);

	// StackWalk64() may modify context record passed to it, so we will
	// use a copy.
	CONTEXT context_record = *pExceptionInfo->ContextRecord;
	// Initialize stack walking.
	STACKFRAME64 stack_frame;
	memset(&stack_frame, 0, sizeof(stack_frame));
#if defined(_WIN64)
	int machine_type = IMAGE_FILE_MACHINE_AMD64;
	stack_frame.AddrPC.Offset = context_record.Rip;
	stack_frame.AddrFrame.Offset = context_record.Rbp;
	stack_frame.AddrStack.Offset = context_record.Rsp;
#else
	int machine_type = IMAGE_FILE_MACHINE_I386;
	stack_frame.AddrPC.Offset = context_record.Eip;
	stack_frame.AddrFrame.Offset = context_record.Ebp;
	stack_frame.AddrStack.Offset = context_record.Esp;
#endif
	stack_frame.AddrPC.Mode = AddrModeFlat;
	stack_frame.AddrFrame.Mode = AddrModeFlat;
	stack_frame.AddrStack.Mode = AddrModeFlat;


	fprintf(stderr, "Exception occurred: 0x%lx\n", pExceptionInfo->ExceptionRecord->ExceptionCode);
	// fprintf(stderr, "Exception occurred: %i\n", pExceptionInfo->ExceptionRecord->NumberParameters);
	// for (int j = 0; j < pExceptionInfo->ExceptionRecord->NumberParameters; ++j) {
	// 	fprintf(stderr, "0x%I64x ", pExceptionInfo->ExceptionRecord->ExceptionInformation[j]);
	//   fprintf(stderr, "%s\n", typeid((void*)pExceptionInfo->ExceptionRecord->ExceptionInformation[j]).name());
	// }
	// type_info *t = (type_info*)(pExceptionInfo->ExceptionRecord->ExceptionInformation[2] + 0x10000);
	// fprintf(stderr, "%p\n", t);
	// fprintf(stderr, "%s\n", t->name());
	// fprintf(stderr, "%s\n", ((std::exception*)pExceptionInfo->ExceptionRecord->ExceptionInformation[1])->what());
	// fprintf(stderr, "%p\n", dynamic_cast<std::exception*>((void*)pExceptionInfo->ExceptionRecord->ExceptionInformation[1]));
	// fprintf(stderr, "%s\n", ((type_info*)pExceptionInfo->ExceptionRecord->ExceptionInformation[2])->name());
	// fprintf(stderr, "%s\n", ((type_info*)pExceptionInfo->ExceptionRecord->ExceptionInformation[3])->name());
	fprintf(stderr, "Stack trace (most recent call first):\n");

	int i = 1;
	while (StackWalk64(machine_type,
		GetCurrentProcess(),
		GetCurrentThread(),
		&stack_frame,
		&context_record,
		NULL,
		&SymFunctionTableAccess64,
		&SymGetModuleBase64,
		NULL)) {
		DWORD64 modulebase = SymGetModuleBase64(process, stack_frame.AddrPC.Offset);

		fprintf(stderr, "  [%i] ", i);
		i++;

		IMAGEHLP_MODULE64 moduleInfo;
		moduleInfo = {};
		moduleInfo.SizeOfStruct = sizeof(moduleInfo);
		if (::SymGetModuleInfo64(process, modulebase, &moduleInfo)) {
			if (moduleInfo.ImageName) {
				char* basename = moduleInfo.ImageName;
				while (true) {
					char* next = strchr(basename, '\\');
					if (next) {
						basename = next + 1;
					} else {
						break;
					}
				}
				fprintf(stderr, "%s", basename);
			} else if (moduleInfo.ModuleName) {
				fprintf(stderr, "%s", moduleInfo.ModuleName);
			} else {
				fprintf(stderr, "???");
			}
		} else {
			fprintf(stderr, "???");
		}

		fprintf(stderr, " ! ");

		char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
		PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		symbol->MaxNameLen = MAX_SYM_NAME;

		DWORD64 displacement = 0;
		if (SymFromAddr(process, (DWORD64)stack_frame.AddrPC.Offset, &displacement, symbol)) {
			fprintf(stderr, "%s ", symbol->Name);

			IMAGEHLP_LINE64 lineInfo;
			lineInfo = {};
			lineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

			DWORD linedisplacement = 0;
			if (::SymGetLineFromAddr64(process, (DWORD64)stack_frame.AddrPC.Offset, &linedisplacement, &lineInfo)) {
				char* basename = lineInfo.FileName;
				while (true) {
					char* next = strchr(basename, '\\');
					if (next) {
						basename = next + 1;
					} else {
						break;
					}
				}
				fprintf(stderr, "in %s at line %i", basename, lineInfo.LineNumber);
			} else {
				fprintf(stderr, "+ 0x%I64x", displacement);
			}
		} else {
			fprintf(stderr, "0x%I64x", stack_frame.AddrPC.Offset);
		}
		fprintf(stderr, "\n");
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

#else // _WIN32

#include <dlfcn.h>
#include <execinfo.h>
#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
#ifdef _WIN32
	SetUnhandledExceptionFilter(TopLevelExceptionHandler);
#else
	signal(SIGABRT, signalhandler);
	signal(SIGBUS, signalhandler);
	signal(SIGFPE, signalhandler);
	signal(SIGILL, signalhandler);
	signal(SIGSEGV, signalhandler);
	signal(SIGTRAP, signalhandler);
#endif
}