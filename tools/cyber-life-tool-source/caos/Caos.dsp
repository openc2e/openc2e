# Microsoft Developer Studio Project File - Name="Caos" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Caos - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Caos.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Caos.mak" CFG="Caos - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Caos - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Caos - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Caos - Win32 Release no opt" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/C2e/Code/tools/CAOS", RXAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Caos - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "NO_ERROR_MESSAGE_HANDLER" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 CTLLicense.lib Shlwapi.lib zlib.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Caos - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "NO_ERROR_MESSAGE_HANDLER" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 CTLLicense.lib Shlwapi.lib zlib.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "Caos - Win32 Release no opt"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Caos___Win32_Release_no_opt"
# PROP BASE Intermediate_Dir "Caos___Win32_Release_no_opt"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Caos___Win32_Release_no_opt"
# PROP Intermediate_Dir "Caos___Win32_Release_no_opt"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "NO_ERROR_MESSAGE_HANDLER" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "NO_ERROR_MESSAGE_HANDLER" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 CTLLicense.lib Shlwapi.lib zlib.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 CTLLicense.lib Shlwapi.lib zlib.lib /nologo /subsystem:windows /machine:I386

!ENDIF 

# Begin Target

# Name "Caos - Win32 Release"
# Name "Caos - Win32 Debug"
# Name "Caos - Win32 Release no opt"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\engine\Caos\AutoDocumentationTable.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Caos.cpp
# End Source File
# Begin Source File

SOURCE=.\Caos.rc
# End Source File
# Begin Source File

SOURCE=.\CaosChildFrame.cpp
# End Source File
# Begin Source File

SOURCE=..\..\engine\Caos\CAOSDescription.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\ChildFrame.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\clientside.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\common\Configurator.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\engine\CreaturesArchive.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\EditQuickMacros.cpp
# End Source File
# Begin Source File

SOURCE=.\FindDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\GameInterface.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\GotoDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\MacroEditCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\MacroParse.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrame.cpp
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

SOURCE=.\ReplaceDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptoriumChildFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptoriumTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\..\common\StringFuncs.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\engine\Caos\TableSpec.cpp
# SUBTRACT CPP /YX /Yc /Yu
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

SOURCE=.\Caos.h
# End Source File
# Begin Source File

SOURCE=.\CaosChildFrame.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\Caos\CAOSDescription.h
# End Source File
# Begin Source File

SOURCE=.\ChildFrame.h
# End Source File
# Begin Source File

SOURCE=..\..\common\clientside.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\CreaturesArchive.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\CreaturesFile.h
# End Source File
# Begin Source File

SOURCE=.\CTLLicense.h
# End Source File
# Begin Source File

SOURCE=.\EditQuickMacros.h
# End Source File
# Begin Source File

SOURCE=..\..\common\File.h
# End Source File
# Begin Source File

SOURCE=.\FindDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\common\GameInterface.h
# End Source File
# Begin Source File

SOURCE=.\GotoDialog.h
# End Source File
# Begin Source File

SOURCE=.\MacroEditCtrl.h
# End Source File
# Begin Source File

SOURCE=.\MacroParse.h
# End Source File
# Begin Source File

SOURCE=.\MainFrame.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\Caos\OpSpec.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\PersistentObject.h
# End Source File
# Begin Source File

SOURCE=.\ReplaceDialog.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\ScriptoriumChildFrame.h
# End Source File
# Begin Source File

SOURCE=.\ScriptoriumTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\..\common\WhichEngine.h
# End Source File
# Begin Source File

SOURCE=..\..\common\WindowState.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\avi1.bin
# End Source File
# Begin Source File

SOURCE=.\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\caos.ico
# End Source File
# Begin Source File

SOURCE=.\res\Caos.ico
# End Source File
# Begin Source File

SOURCE=.\res\Caos.rc2
# End Source File
# Begin Source File

SOURCE=.\cos.ico
# End Source File
# Begin Source File

SOURCE=.\cosfile.ico
# End Source File
# Begin Source File

SOURCE=.\dde.ico
# End Source File
# Begin Source File

SOURCE=.\dde1.ico
# End Source File
# Begin Source File

SOURCE=.\dde2.ico
# End Source File
# Begin Source File

SOURCE=.\edit.ico
# End Source File
# Begin Source File

SOURCE=.\error.ico
# End Source File
# Begin Source File

SOURCE=.\error1.ico
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\idr_main.ico
# End Source File
# Begin Source File

SOURCE=.\imagelist.bmp
# End Source File
# Begin Source File

SOURCE=.\main.ico
# End Source File
# Begin Source File

SOURCE=.\mainfram.bmp
# End Source File
# Begin Source File

SOURCE=.\moopy.ico
# End Source File
# Begin Source File

SOURCE=.\new.bmp
# End Source File
# Begin Source File

SOURCE=.\NewCAOS.ico
# End Source File
# Begin Source File

SOURCE=.\NewCOSFile.ico
# End Source File
# Begin Source File

SOURCE=.\open.bmp
# End Source File
# Begin Source File

SOURCE=.\output.ico
# End Source File
# Begin Source File

SOURCE=.\scriptorium.ico
# End Source File
# Begin Source File

SOURCE=.\splash.bmp
# End Source File
# Begin Source File

SOURCE=.\splash_no_version.bmp
# End Source File
# Begin Source File

SOURCE=.\test.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# End Target
# End Project
