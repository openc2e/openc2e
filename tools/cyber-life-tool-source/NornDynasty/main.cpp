// nettest.cpp is the main file

// #define SINGLE_STRESS_THREAD

#ifdef _MSC_VER
#pragma warning(disable:4786 4503)
#endif

#include <iostream>
#include <sstream>
#include <fstream>

#ifdef _WIN32
	#include <windows.h>
#endif

#include <Python.h>
#include <DSNetManager.h>

#include "../../common/FileFuncs.h"
#include "dstestmodule.h"

void main()
{
    // Pass argv[0] to the Python interpreter - Not!
    Py_SetProgramName("Norn stress");

    // Initialize the Python interpreter.  Required.
    Py_Initialize();

    // Add a static module 
    initdstest();

	// Read in python file
	std::string pythonCode;
	{
		std::string pythonFile = "test.py";
		std::ifstream in(pythonFile.c_str(), std::ios::in); // not binary, so CR/LF is converted
		if (!in.good())
			std::cerr << "Failed to open " << pythonFile;
		pythonCode.resize(FileSize(pythonFile.c_str()));
		in.read(&pythonCode[0], FileSize(pythonFile.c_str()));
	}
	
	// Execute it#
	int sts = PyRun_SimpleString(const_cast<char*>(pythonCode.c_str()));

	std::cout << "Python returned code " << sts << std::endl;
/*
	std::cout << "Type something and press return to exit" << std::endl;
	std::string wait;
	std::cin >> wait;
	*/
}

