#ifndef INSTALL_LIB_API_H
#define INSTALL_LIB_API_H

#ifdef _MSC_VER
	// VC++
	#ifdef INSTALL_LIB_DLL_EXPORT
		// For building the DLL
		#define INSTALL_LIB_API __declspec(dllexport)
	#elif INSTALL_LIB_DLL_IMPORT
		// Program using the DLL
		#define INSTALL_LIB_API __declspec(dllimport)
	#else // used to be INSTALL_LIB_STATIC_LINK
		// Static - this is the default.
		// Add the *.cpp files to your project.
		#define INSTALL_LIB_API
	#endif
#else
	// Other compilers not done
	#error "Dll import and export for new compiler"
#endif

#endif
