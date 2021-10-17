# Microsoft Developer Studio Project File - Name="CAOSDebugger" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=CAOSDebugger - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CAOSDebugger.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CAOSDebugger.mak" CFG="CAOSDebugger - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CAOSDebugger - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "CAOSDebugger - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/C2e/Code/tools/CAOSDebugger", BULAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CAOSDebugger - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /Od /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "NO_ERROR_MESSAGE_HANDLER" /D "_CAOSDEBUGGER" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib Shlwapi.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "CAOSDebugger - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "NO_ERROR_MESSAGE_HANDLER" /D "_CAOSDEBUGGER" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib Shlwapi.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "CAOSDebugger - Win32 Release"
# Name "CAOSDebugger - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "ZLib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\common\zlib113\adler32.c
# End Source File
# Begin Source File

SOURCE=..\..\common\zlib113\compress.c
# End Source File
# Begin Source File

SOURCE=..\..\common\zlib113\crc32.c
# End Source File
# Begin Source File

SOURCE=..\..\common\zlib113\deflate.c
# End Source File
# Begin Source File

SOURCE=..\..\common\zlib113\gzio.c
# End Source File
# Begin Source File

SOURCE=..\..\common\zlib113\infblock.c
# End Source File
# Begin Source File

SOURCE=..\..\common\zlib113\infcodes.c
# End Source File
# Begin Source File

SOURCE=..\..\common\zlib113\inffast.c
# End Source File
# Begin Source File

SOURCE=..\..\common\zlib113\inflate.c
# End Source File
# Begin Source File

SOURCE=..\..\common\zlib113\inftrees.c
# End Source File
# Begin Source File

SOURCE=..\..\common\zlib113\infutil.c
# End Source File
# Begin Source File

SOURCE=..\..\common\zlib113\maketree.c
# End Source File
# Begin Source File

SOURCE=..\..\common\zlib113\trees.c
# End Source File
# Begin Source File

SOURCE=..\..\common\zlib113\uncompr.c
# End Source File
# Begin Source File

SOURCE=..\..\common\zlib113\zutil.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\AgentTree.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\..\engine\Caos\AutoDocumentationTable.cpp
# End Source File
# Begin Source File

SOURCE=.\CAOSDebugger.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\CAOSDebugger.rc
# End Source File
# Begin Source File

SOURCE=..\..\engine\Caos\CAOSDescription.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\common\Catalogue.cpp
# End Source File
# Begin Source File

SOURCE=..\..\engine\Classifier.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\common\clientside.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\engine\CreaturesArchive.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\common\GameInterface.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\caos\MacroEditCtrl.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\caos\MacroParse.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\MonitorDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\engine\Caos\OpSpec.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\engine\PersistentObject.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SplitterBar.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\..\engine\Caos\TableSpec.cpp
# End Source File
# Begin Source File

SOURCE=.\WatchDialog.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\WatchStreamer.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\..\common\WhichEngine.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\common\WindowState.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AgentTree.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\Caos\AutoDocumentationTable.h
# End Source File
# Begin Source File

SOURCE=.\CAOSDebugger.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\Caos\CAOSDescription.h
# End Source File
# Begin Source File

SOURCE=..\..\common\Catalogue.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\Classifier.h
# End Source File
# Begin Source File

SOURCE=..\..\common\clientside.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\CreaturesArchive.h
# End Source File
# Begin Source File

SOURCE=..\..\common\zlib113\deflate.h
# End Source File
# Begin Source File

SOURCE=..\..\common\GameInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\common\zlib113\infblock.h
# End Source File
# Begin Source File

SOURCE=..\..\common\zlib113\infcodes.h
# End Source File
# Begin Source File

SOURCE=..\..\common\zlib113\inffast.h
# End Source File
# Begin Source File

SOURCE=..\..\common\zlib113\inffixed.h
# End Source File
# Begin Source File

SOURCE=..\..\common\zlib113\inftrees.h
# End Source File
# Begin Source File

SOURCE=..\..\common\zlib113\infutil.h
# End Source File
# Begin Source File

SOURCE=..\caos\MacroEditCtrl.h
# End Source File
# Begin Source File

SOURCE=..\caos\MacroParse.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MonitorDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\Caos\OpSpec.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\PersistentObject.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SplitterBar.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\Caos\TableSpec.h
# End Source File
# Begin Source File

SOURCE=..\..\common\zlib113\trees.h
# End Source File
# Begin Source File

SOURCE=.\WatchDialog.h
# End Source File
# Begin Source File

SOURCE=.\WatchStreamer.h
# End Source File
# Begin Source File

SOURCE=..\..\common\WhichEngine.h
# End Source File
# Begin Source File

SOURCE=..\..\common\WindowState.h
# End Source File
# Begin Source File

SOURCE=..\..\common\zlib113\zconf.h
# End Source File
# Begin Source File

SOURCE=..\..\common\zlib113\zlib.h
# End Source File
# Begin Source File

SOURCE=..\..\common\zlib113\zutil.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CAOSDebugger.ico
# End Source File
# Begin Source File

SOURCE=.\res\CAOSDebugger.rc2
# End Source File
# Begin Source File

SOURCE=.\res\imagelist.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Group "Stuff to make it compile"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\common\FileLocaliser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\FileLocaliser.h
# End Source File
# Begin Source File

SOURCE=..\..\common\SimpleLexer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\SimpleLexer.h
# End Source File
# End Group
# End Target
# End Project
