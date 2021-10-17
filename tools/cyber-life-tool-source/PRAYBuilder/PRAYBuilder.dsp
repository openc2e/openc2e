# Microsoft Developer Studio Project File - Name="PRAYBuilder" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=PRAYBuilder - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PRAYBuilder.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PRAYBuilder.mak" CFG="PRAYBuilder - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PRAYBuilder - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "PRAYBuilder - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Code/tools/PRAYBuilder", MRBAAAAA"
# PROP Scc_LocalPath "."
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "PRAYBuilder - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "PRAY_BUILDER" /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 zlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "PRAYBuilder - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "PRAY_BUILDER" /YX /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 zlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "PRAYBuilder - Win32 Release"
# Name "PRAYBuilder - Win32 Debug"
# Begin Group "PRAY Files"

# PROP Default_Filter ""
# Begin Group "Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Common\PRAYFiles\PrayChunk.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\PRAYFiles\PrayException.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\PRAYFiles\PrayManager.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\PRAYFiles\PrayStructs.h
# End Source File
# Begin Source File

SOURCE=..\..\common\PRAYFiles\StringIntGroup.h
# End Source File
# End Group
# Begin Group "CPPs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Common\PRAYFiles\PrayChunk.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\PRAYFiles\PrayManager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\PRAYFiles\StringIntGroup.cpp
# End Source File
# End Group
# End Group
# Begin Group "Other Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Common\C2eTypes.h
# End Source File
# Begin Source File

SOURCE=..\..\common\FileFuncs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\FileFuncs.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\FileLocaliser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\FileLocaliser.h
# End Source File
# End Group
# Begin Group "Lexer Stuffs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\common\SimpleLexer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\SimpleLexer.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Interface.cpp
# End Source File
# End Target
# End Project
