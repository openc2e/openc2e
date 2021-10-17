# Microsoft Developer Studio Project File - Name="C2E Room Editor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=C2E Room Editor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "C2ERoomEditor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "C2ERoomEditor.mak" CFG="C2E Room Editor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "C2E Room Editor - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "C2E Room Editor - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/C2e/Code/tools/Room Editor", TFKAAAAA"
# PROP Scc_LocalPath "."
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "C2E Room Editor - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib ..\caos\ctllicense.lib /nologo /subsystem:windows /machine:I386 /out:"Release/MapEditor.exe"

!ELSEIF  "$(CFG)" == "C2E Room Editor - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib ..\caos\ctllicense.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/MapEditor.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "C2E Room Editor - Win32 Release"
# Name "C2E Room Editor - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Action.cpp
# End Source File
# Begin Source File

SOURCE=.\Back2Spr.cpp
# End Source File
# Begin Source File

SOURCE=.\BackgroundDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BackgroundFileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\C2ERoomEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\C2ERoomEditor.rc
# End Source File
# Begin Source File

SOURCE=.\C2ERoomEditorDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\C2ERoomEditorView.cpp
# End Source File
# Begin Source File

SOURCE=.\CAPropertiesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CARates.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\clientside.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\ColourRange.cpp
# End Source File
# Begin Source File

SOURCE=.\ComboList.cpp
# End Source File
# Begin Source File

SOURCE=.\EmptyChildDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FloorValuesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Game.cpp
# End Source File
# Begin Source File

SOURCE=.\HeightCheckDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Metaroom.cpp
# End Source File
# Begin Source File

SOURCE=.\MusicDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PropertiesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PropertyListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\PropertyType.cpp
# End Source File
# Begin Source File

SOURCE=.\PropertyTypeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PropertyTypesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\REException.cpp
# End Source File
# Begin Source File

SOURCE=.\Room.cpp
# End Source File
# Begin Source File

SOURCE=.\RoomCA.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\RoomPropertiesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SwitchMetaroomDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TipDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Tool.cpp
# End Source File
# Begin Source File

SOURCE=.\utils.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewParams.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\WhichEngine.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\World.cpp
# End Source File
# Begin Source File

SOURCE=.\WorldSizeDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Action.h
# End Source File
# Begin Source File

SOURCE=.\BackgroundDlg.h
# End Source File
# Begin Source File

SOURCE=.\BackgroundFileDlg.h
# End Source File
# Begin Source File

SOURCE=.\BMPToSprite.h
# End Source File
# Begin Source File

SOURCE=.\C2ERoomEditor.h
# End Source File
# Begin Source File

SOURCE=.\C2ERoomEditorDoc.h
# End Source File
# Begin Source File

SOURCE=.\C2ERoomEditorView.h
# End Source File
# Begin Source File

SOURCE=.\CAPropertiesDlg.h
# End Source File
# Begin Source File

SOURCE=.\CARates.h
# End Source File
# Begin Source File

SOURCE=.\Cheese.h
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=..\..\common\clientside.h
# End Source File
# Begin Source File

SOURCE=.\ColourRange.h
# End Source File
# Begin Source File

SOURCE=.\ComboList.h
# End Source File
# Begin Source File

SOURCE=.\Door.h
# End Source File
# Begin Source File

SOURCE=.\EmptyChildDlg.h
# End Source File
# Begin Source File

SOURCE=.\FloorValuesDlg.h
# End Source File
# Begin Source File

SOURCE=.\Game.h
# End Source File
# Begin Source File

SOURCE=.\handle.h
# End Source File
# Begin Source File

SOURCE=.\HeightCheckDlg.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MetaRoom.h
# End Source File
# Begin Source File

SOURCE=.\MusicDlg.h
# End Source File
# Begin Source File

SOURCE=.\OptionsDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProgDlg.h
# End Source File
# Begin Source File

SOURCE=.\PropertiesDlg.h
# End Source File
# Begin Source File

SOURCE=.\PropertyListBox.h
# End Source File
# Begin Source File

SOURCE=.\PropertyType.h
# End Source File
# Begin Source File

SOURCE=.\PropertyTypeDlg.h
# End Source File
# Begin Source File

SOURCE=.\PropertyTypesDlg.h
# End Source File
# Begin Source File

SOURCE=.\REException.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Room.h
# End Source File
# Begin Source File

SOURCE=.\RoomCA.h
# End Source File
# Begin Source File

SOURCE=.\RoomPropertiesDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SwitchMetaroomDlg.h
# End Source File
# Begin Source File

SOURCE=.\TipDlg.h
# End Source File
# Begin Source File

SOURCE=.\Tool.h
# End Source File
# Begin Source File

SOURCE=.\utils.h
# End Source File
# Begin Source File

SOURCE=.\ViewParams.h
# End Source File
# Begin Source File

SOURCE=..\..\common\WhichEngine.h
# End Source File
# Begin Source File

SOURCE=.\world.h
# End Source File
# Begin Source File

SOURCE=.\WorldSizeDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\C2ERoomEditor.ico
# End Source File
# Begin Source File

SOURCE=.\res\C2ERoomEditor.rc2
# End Source File
# Begin Source File

SOURCE=.\res\C2ERoomEditorDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\cur00001.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\litebulb.bmp
# End Source File
# Begin Source File

SOURCE=.\res\MapEditor256.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\zoom.cur
# End Source File
# End Group
# Begin Source File

SOURCE=.\C2ERoomEditor.reg
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\Tips.txt
# End Source File
# End Target
# End Project
