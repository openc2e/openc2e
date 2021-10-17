# Microsoft Developer Studio Project File - Name="NornDynasty" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=NornDynasty - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "NornDynasty.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NornDynasty.mak" CFG="NornDynasty - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NornDynasty - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "NornDynasty - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "NornDynasty - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../../Babel/BabelCloak" /I "../../../Babel/BabelClient" /I "../../../Babel/BabelCommon" /I "../../../Babel/MySQL/include" /I "../../../c2e/common" /D "NDEBUG" /D "O_WIN32" /D "NON_LOCALISED_ERRORS" /D "_CONSOLE" /D "MD5_NO_GAME_ENTROPY" /D "ISOLATED_HISTORY" /D "WIN32" /D "_MBCS" /D PRODUCTCODE=2 /D CLIENTVERSION=1 /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 wininet.lib python20.lib rpcrt4.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib gtk-1.3.lib gdk-1.3.lib glib-1.3.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "NornDynasty - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../c2e/common" /I "../../../Babel/BabelClient" /I "../../../Babel/BabelCommon" /I "../../../Babel/BabelCloak" /I "../../../Babel/MySQL/include" /D "_DEBUG" /D "O_WIN32" /D "NON_LOCALISED_ERRORS" /D "_CONSOLE" /D "MD5_NO_GAME_ENTROPY" /D "ISOLATED_HISTORY" /D "WIN32" /D "_MBCS" /D PRODUCTCODE=2 /D CLIENTVERSION=1 /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wininet.lib rpcrt4.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib gtk-1.3.lib gdk-1.3.lib glib-1.3.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "NornDynasty - Win32 Release"
# Name "NornDynasty - Win32 Debug"
# Begin Group "BabelCloak"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\BasicException.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\BasicException.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\BoolPack.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\BoolPack.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\Defines.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\DSNetManager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\DSNetManager.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\DSNetMessages.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\FileFuncs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\FileFuncs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\MessageThread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\MessageThread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\MessageThread.inl
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\NetDebugListUsers.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\NetDebugListUsers.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\NetDirectLink.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\NetDirectLink.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\NetLogInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\NetManager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\NetManager.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\NetMemoryPack.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\NetMemoryPack.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\NetMemoryPack.inl
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\NetMemoryUnpack.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\NetMemoryUnpack.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\NetMessages.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\NetUtilities.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\NetUtilities.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\QueuedMessage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\QueuedMessage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\QueuedMessageClient.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\QueuedMessageClient.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\SimpleMutex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\SimpleMutex.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\Thread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\Thread.inl
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\UserId.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\UserId.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\BabelCloak\UserId.inl
# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\common\FileScanner.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\FileScanner.h
# End Source File
# Begin Source File

SOURCE=..\..\common\ProfanityFilter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\ProfanityFilter.h
# End Source File
# Begin Source File

SOURCE=..\..\common\StringFuncs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\StringFuncs.h
# End Source File
# End Group
# Begin Group "HistoryFeed"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\server\HistoryFeed\BabelHistoryFeed.cpp
# End Source File
# Begin Source File

SOURCE=..\..\server\HistoryFeed\BabelHistoryFeed.h
# End Source File
# Begin Source File

SOURCE=..\..\server\HistoryFeed\HistoryFeed.cpp
# End Source File
# Begin Source File

SOURCE=..\..\server\HistoryFeed\HistoryFeed.h
# End Source File
# Begin Source File

SOURCE=..\..\server\HistoryFeed\HistoryTransferOut.cpp
# End Source File
# Begin Source File

SOURCE=..\..\server\HistoryFeed\HistoryTransferOut.h
# End Source File
# Begin Source File

SOURCE=..\..\server\HistoryFeed\SQLInterface.cpp
# End Source File
# Begin Source File

SOURCE=..\..\server\HistoryFeed\SQLInterface.h
# End Source File
# End Group
# Begin Group "Engine"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\engine\Creature\History\CreatureHistory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\engine\Creature\History\CreatureHistory.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\CreaturesArchive.cpp
# End Source File
# Begin Source File

SOURCE=..\..\engine\CreaturesArchive.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\Creature\History\HistoryStore.cpp
# End Source File
# Begin Source File

SOURCE=..\..\engine\Creature\History\HistoryStore.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\Creature\History\LifeEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\engine\Creature\History\LifeEvent.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\PersistentObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\engine\PersistentObject.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\UniqueIdentifier.cpp
# End Source File
# Begin Source File

SOURCE=..\..\engine\UniqueIdentifier.h
# End Source File
# End Group
# Begin Group "libs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\engine\md5.cpp
# End Source File
# Begin Source File

SOURCE=..\..\engine\md5.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\MySQL\lib\debug\libmySQL.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\MySQL\lib\debug\zlib.lib
# End Source File
# End Group
# Begin Group "Tech"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Tech\Base\Common\Exception.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Tech\Base\Common\win32\win32_Debug.cpp
# End Source File
# End Group
# Begin Group "main"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DSStress.cpp
# End Source File
# Begin Source File

SOURCE=.\DSStress.h
# End Source File
# Begin Source File

SOURCE=.\dstestmodule.cpp
# End Source File
# Begin Source File

SOURCE=.\dstestmodule.h
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\main.h
# End Source File
# Begin Source File

SOURCE=.\NornDynasty.cpp
# End Source File
# Begin Source File

SOURCE=.\NornDynasty.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\test.py
# End Source File
# End Target
# End Project
