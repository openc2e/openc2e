# Microsoft Developer Studio Project File - Name="BiochemistrySet" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=BiochemistrySet - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "BiochemistrySet.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "BiochemistrySet.mak" CFG="BiochemistrySet - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "BiochemistrySet - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "BiochemistrySet - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/C2e/Code/tools/BiochemistrySet/BiochemistrySet", MALAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "BiochemistrySet - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 Shlwapi.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "BiochemistrySet - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Shlwapi.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "BiochemistrySet - Win32 Release"
# Name "BiochemistrySet - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BaseDlg.cpp

!IF  "$(CFG)" == "BiochemistrySet - Win32 Release"

!ELSEIF  "$(CFG)" == "BiochemistrySet - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\BiochemistrySet.cpp

!IF  "$(CFG)" == "BiochemistrySet - Win32 Release"

!ELSEIF  "$(CFG)" == "BiochemistrySet - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\BiochemistrySet.rc
# End Source File
# Begin Source File

SOURCE=.\BiochemistrySetDlg.cpp

!IF  "$(CFG)" == "BiochemistrySet - Win32 Release"

!ELSEIF  "$(CFG)" == "BiochemistrySet - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\Catalogue.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\ChemNames.cpp

!IF  "$(CFG)" == "BiochemistrySet - Win32 Release"

!ELSEIF  "$(CFG)" == "BiochemistrySet - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\clientside.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\CreatureMoniker.cpp

!IF  "$(CFG)" == "BiochemistrySet - Win32 Release"

!ELSEIF  "$(CFG)" == "BiochemistrySet - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DialogSpec.cpp

!IF  "$(CFG)" == "BiochemistrySet - Win32 Release"

!ELSEIF  "$(CFG)" == "BiochemistrySet - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FavouriteClass.cpp

!IF  "$(CFG)" == "BiochemistrySet - Win32 Release"

!ELSEIF  "$(CFG)" == "BiochemistrySet - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FavouriteName.cpp

!IF  "$(CFG)" == "BiochemistrySet - Win32 Release"

!ELSEIF  "$(CFG)" == "BiochemistrySet - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\FileFuncs.cpp

!IF  "$(CFG)" == "BiochemistrySet - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "BiochemistrySet - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\FileLocaliser.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\common\GameInterface.cpp

!IF  "$(CFG)" == "BiochemistrySet - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "BiochemistrySet - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\Grapher.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\GrapherDlg.cpp

!IF  "$(CFG)" == "BiochemistrySet - Win32 Release"

!ELSEIF  "$(CFG)" == "BiochemistrySet - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\InjectionDlg.cpp

!IF  "$(CFG)" == "BiochemistrySet - Win32 Release"

!ELSEIF  "$(CFG)" == "BiochemistrySet - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\LabelThing.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\LoggerDlg.cpp

!IF  "$(CFG)" == "BiochemistrySet - Win32 Release"

!ELSEIF  "$(CFG)" == "BiochemistrySet - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\OrganDlg.cpp

!IF  "$(CFG)" == "BiochemistrySet - Win32 Release"

!ELSEIF  "$(CFG)" == "BiochemistrySet - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ParametersDlg.cpp

!IF  "$(CFG)" == "BiochemistrySet - Win32 Release"

!ELSEIF  "$(CFG)" == "BiochemistrySet - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\SimpleLexer.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
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

SOURCE=.\BaseDlg.h
# End Source File
# Begin Source File

SOURCE=.\BiochemistrySet.h
# End Source File
# Begin Source File

SOURCE=.\BiochemistrySetDlg.h
# End Source File
# Begin Source File

SOURCE=..\..\common\Catalogue.h
# End Source File
# Begin Source File

SOURCE=.\ChemNames.h
# End Source File
# Begin Source File

SOURCE=..\..\common\clientside.h
# End Source File
# Begin Source File

SOURCE=.\CreatureMoniker.h
# End Source File
# Begin Source File

SOURCE=.\DialogSpec.h
# End Source File
# Begin Source File

SOURCE=.\FavouriteClass.h
# End Source File
# Begin Source File

SOURCE=.\FavouriteName.h
# End Source File
# Begin Source File

SOURCE=..\..\common\FileFuncs.h
# End Source File
# Begin Source File

SOURCE=..\..\common\GameInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\common\Grapher.h
# End Source File
# Begin Source File

SOURCE=.\Grapher.h
# End Source File
# Begin Source File

SOURCE=.\GrapherDlg.h
# End Source File
# Begin Source File

SOURCE=.\InjectionDlg.h
# End Source File
# Begin Source File

SOURCE=..\..\common\LabelThing.h
# End Source File
# Begin Source File

SOURCE=.\LoggerDlg.h
# End Source File
# Begin Source File

SOURCE=.\OrganDlg.h
# End Source File
# Begin Source File

SOURCE=.\ParametersDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\..\common\WhichEngine.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\BiochemistrySet.ico
# End Source File
# Begin Source File

SOURCE=.\res\BiochemistrySet.rc2
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_main.ico
# End Source File
# Begin Source File

SOURCE=.\res\SmallScience.bmp
# End Source File
# End Group
# End Target
# End Project
