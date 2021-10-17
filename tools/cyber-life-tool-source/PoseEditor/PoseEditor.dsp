# Microsoft Developer Studio Project File - Name="PoseEditor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=PoseEditor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PoseEditor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PoseEditor.mak" CFG="PoseEditor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PoseEditor - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "PoseEditor - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/C2e/Code/tools/PoseEditor", BCLAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PoseEditor - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
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
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "PoseEditor - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
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
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "PoseEditor - Win32 Release"
# Name "PoseEditor - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BlueRadio.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\creaturepicture.cpp
# End Source File
# Begin Source File

SOURCE=.\creaturepicture2.cpp
# End Source File
# Begin Source File

SOURCE=.\GaitDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Gene.cpp
# End Source File
# Begin Source File

SOURCE=.\GeneNames.cpp
# End Source File
# Begin Source File

SOURCE=.\Genome.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\NewGaitDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\NewPoseDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PoseCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\PoseDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PoseEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\PoseEditor.rc
# End Source File
# Begin Source File

SOURCE=.\PoseEditorDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\PoseEditorView.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Appearance.h
# End Source File
# Begin Source File

SOURCE=.\BlueRadio.h
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\CreaturePic2.h
# End Source File
# Begin Source File

SOURCE=.\creaturepicture.h
# End Source File
# Begin Source File

SOURCE=.\creaturepicture2.h
# End Source File
# Begin Source File

SOURCE=.\CTLLicense.h
# End Source File
# Begin Source File

SOURCE=.\GaitDlg.h
# End Source File
# Begin Source File

SOURCE=.\Gene.h
# End Source File
# Begin Source File

SOURCE=.\Genome.h
# End Source File
# Begin Source File

SOURCE=.\handle.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\NewGaitDlg.h
# End Source File
# Begin Source File

SOURCE=.\NewPoseDlg.h
# End Source File
# Begin Source File

SOURCE=.\PoseCommand.h
# End Source File
# Begin Source File

SOURCE=.\PoseDlg.h
# End Source File
# Begin Source File

SOURCE=.\PoseEditor.h
# End Source File
# Begin Source File

SOURCE=.\PoseEditorDoc.h
# End Source File
# Begin Source File

SOURCE=.\PoseEditorView.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PoseEditor.ico
# End Source File
# Begin Source File

SOURCE=.\res\PoseEditor.rc2
# End Source File
# Begin Source File

SOURCE=.\res\PoseEditorDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\CTLLicense.dll
# End Source File
# Begin Source File

SOURCE=.\PoseEditor.reg
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\CTLLicense.lib
# End Source File
# End Target
# End Project
# Section PoseEditor : {843EACE7-D2D6-11D2-9D30-0090271EEABE}
# 	2:5:Class:CCreaturePicture2
# 	2:10:HeaderFile:creaturepicture2.h
# 	2:8:ImplFile:creaturepicture2.cpp
# End Section
# Section PoseEditor : {11DF2DD2-A937-11D2-9D30-0090271EEABE}
# 	2:21:DefaultSinkHeaderFile:creaturepicture.h
# 	2:16:DefaultSinkClass:CCreaturePicture
# End Section
# Section PoseEditor : {843EACE9-D2D6-11D2-9D30-0090271EEABE}
# 	2:21:DefaultSinkHeaderFile:creaturepicture2.h
# 	2:16:DefaultSinkClass:CCreaturePicture2
# End Section
# Section PoseEditor : {11DF2DD0-A937-11D2-9D30-0090271EEABE}
# 	2:5:Class:CCreaturePicture
# 	2:10:HeaderFile:creaturepicture.h
# 	2:8:ImplFile:creaturepicture.cpp
# End Section
