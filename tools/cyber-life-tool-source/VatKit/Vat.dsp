# Microsoft Developer Studio Project File - Name="Vat" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Vat - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Vat.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Vat.mak" CFG="Vat - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Vat - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Vat - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Vat - Win32 Release with debug symbols" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Creatures 3/Vat", RKLAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Vat - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /Gi /GR /GX /Ox /Ot /Oa /Ow /Og /Oi /Op /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "C2E_MODULE_NO" /D "VAT_KIT" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib zdll.lib /nologo /subsystem:windows /profile /debug /machine:I386 /out:"c:\program files\gameware development\Creatures exodus\docking station\Vat.exe"

!ELSEIF  "$(CFG)" == "Vat - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "C2E_MODULE_NO" /D "VAT_KIT" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ddraw.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib zdll.lib /nologo /subsystem:windows /debug /machine:I386 /out:"c:\program files\gameware development\Creatures exodus\docking station\Vat.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "Vat - Win32 Release with debug symbols"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Vat___Win32_Release_with_debug_symbols"
# PROP BASE Intermediate_Dir "Vat___Win32_Release_with_debug_symbols"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Vat___Win32_Release_with_debug_symbols"
# PROP Intermediate_Dir "Vat___Win32_Release_with_debug_symbols"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gi /GX /Ox /Ot /Oa /Ow /Og /Oi /Op /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /Gi /GX /Zi /Ox /Ot /Oa /Ow /Og /Oi /Op /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "VAT_KIT" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /profile /debug /machine:I386
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib zlib.lib /nologo /subsystem:windows /profile /debug /machine:I386

!ENDIF 

# Begin Target

# Name "Vat - Win32 Release"
# Name "Vat - Win32 Debug"
# Name "Vat - Win32 Release with debug symbols"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BrainDlgContainer.cpp
# End Source File
# Begin Source File

SOURCE=.\BrainViewport.cpp
# End Source File
# Begin Source File

SOURCE=.\Edit.cpp
# End Source File
# Begin Source File

SOURCE=.\EnginePatch.cpp
# End Source File
# Begin Source File

SOURCE=.\GeneAgeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GraphDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LobeGraphDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LobeInputDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LobeNames.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\SVCaptions.cpp

!IF  "$(CFG)" == "Vat - Win32 Release"

# ADD CPP /Od

!ELSEIF  "$(CFG)" == "Vat - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vat - Win32 Release with debug symbols"

# ADD BASE CPP /Od
# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SVRuleDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TextViewport.cpp
# End Source File
# Begin Source File

SOURCE=.\ThreshHoldDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Tips.cpp
# End Source File
# Begin Source File

SOURCE=.\VarDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VarGraphDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Viewport.cpp
# End Source File
# Begin Source File

SOURCE=.\WinMain.cpp
# End Source File
# Begin Source File

SOURCE=.\Wnd.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BrainAccess.h
# End Source File
# Begin Source File

SOURCE=.\BrainDlg.h
# End Source File
# Begin Source File

SOURCE=.\BrainDlgContainer.h
# End Source File
# Begin Source File

SOURCE=.\BrainViewport.h
# End Source File
# Begin Source File

SOURCE=.\DlgWnd.h
# End Source File
# Begin Source File

SOURCE=.\Edit.h
# End Source File
# Begin Source File

SOURCE=.\GeneAgeDlg.h
# End Source File
# Begin Source File

SOURCE=.\GraphDlg.h
# End Source File
# Begin Source File

SOURCE=.\LobeDefinition.h
# End Source File
# Begin Source File

SOURCE=.\LobeGraphDlg.h
# End Source File
# Begin Source File

SOURCE=.\LobeInputDlg.h
# End Source File
# Begin Source File

SOURCE=.\LobeNames.h
# End Source File
# Begin Source File

SOURCE=.\MainFrame.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SVCaptions.h
# End Source File
# Begin Source File

SOURCE=.\SVRuleDlg.h
# End Source File
# Begin Source File

SOURCE=.\TextViewport.h
# End Source File
# Begin Source File

SOURCE=.\ThreshHoldDlg.h
# End Source File
# Begin Source File

SOURCE=.\Tips.h
# End Source File
# Begin Source File

SOURCE=.\VarDlg.h
# End Source File
# Begin Source File

SOURCE=.\VarGraphDlg.h
# End Source File
# Begin Source File

SOURCE=.\Vat.h
# End Source File
# Begin Source File

SOURCE=.\Viewport.h
# End Source File
# Begin Source File

SOURCE=.\Wnd.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\Vat.rc
# End Source File
# Begin Source File

SOURCE=.\vptoolbar.bmp
# End Source File
# End Group
# Begin Group "Brain"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\engine\Creature\Brain\Brain.cpp

!IF  "$(CFG)" == "Vat - Win32 Release"

!ELSEIF  "$(CFG)" == "Vat - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vat - Win32 Release with debug symbols"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\engine\Creature\Brain\Brain.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\Creature\Brain\BrainComponent.cpp

!IF  "$(CFG)" == "Vat - Win32 Release"

!ELSEIF  "$(CFG)" == "Vat - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vat - Win32 Release with debug symbols"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\engine\Creature\Brain\BrainComponent.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\Creature\Brain\BrainIO.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\Creature\Brain\Dendrite.cpp

!IF  "$(CFG)" == "Vat - Win32 Release"

!ELSEIF  "$(CFG)" == "Vat - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vat - Win32 Release with debug symbols"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\engine\Creature\Brain\Dendrite.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\creature\Faculty.cpp

!IF  "$(CFG)" == "Vat - Win32 Release"

!ELSEIF  "$(CFG)" == "Vat - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vat - Win32 Release with debug symbols"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\engine\creature\Faculty.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\creature\brain\instinct.cpp

!IF  "$(CFG)" == "Vat - Win32 Release"

!ELSEIF  "$(CFG)" == "Vat - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vat - Win32 Release with debug symbols"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\engine\creature\brain\instinct.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\Creature\Brain\Lobe.cpp

!IF  "$(CFG)" == "Vat - Win32 Release"

!ELSEIF  "$(CFG)" == "Vat - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vat - Win32 Release with debug symbols"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\engine\Creature\Brain\Lobe.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\Creature\Brain\Neuron.cpp

!IF  "$(CFG)" == "Vat - Win32 Release"

!ELSEIF  "$(CFG)" == "Vat - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vat - Win32 Release with debug symbols"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\engine\Creature\Brain\Neuron.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\Creature\Brain\SVRule.cpp

!IF  "$(CFG)" == "Vat - Win32 Release"

!ELSEIF  "$(CFG)" == "Vat - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vat - Win32 Release with debug symbols"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\engine\Creature\Brain\SVRule.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\Creature\Brain\Tract.cpp

!IF  "$(CFG)" == "Vat - Win32 Release"

!ELSEIF  "$(CFG)" == "Vat - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vat - Win32 Release with debug symbols"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\engine\Creature\Brain\Tract.h
# End Source File
# End Group
# Begin Group "Creature"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\engine\Creature\Genome.cpp

!IF  "$(CFG)" == "Vat - Win32 Release"

!ELSEIF  "$(CFG)" == "Vat - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vat - Win32 Release with debug symbols"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\engine\Creature\Genome.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\Creature\GenomeStore.h
# End Source File
# End Group
# Begin Group "Engine"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\engine\Agents\AgentHandle.cpp

!IF  "$(CFG)" == "Vat - Win32 Release"

!ELSEIF  "$(CFG)" == "Vat - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vat - Win32 Release with debug symbols"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\engine\Agents\AgentHandle.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\Creature\Brain\BrainScriptFunctions.cpp

!IF  "$(CFG)" == "Vat - Win32 Release"

!ELSEIF  "$(CFG)" == "Vat - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vat - Win32 Release with debug symbols"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\engine\Creature\Brain\BrainScriptFunctions.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\C2eServices.cpp

!IF  "$(CFG)" == "Vat - Win32 Release"

!ELSEIF  "$(CFG)" == "Vat - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vat - Win32 Release with debug symbols"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\engine\C2eServices.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\Caos\CAOSVar.cpp

!IF  "$(CFG)" == "Vat - Win32 Release"

!ELSEIF  "$(CFG)" == "Vat - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vat - Win32 Release with debug symbols"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\engine\Caos\CAOSVar.h
# End Source File
# Begin Source File

SOURCE=..\..\common\Configurator.cpp

!IF  "$(CFG)" == "Vat - Win32 Release"

!ELSEIF  "$(CFG)" == "Vat - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vat - Win32 Release with debug symbols"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\Configurator.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\CreaturesArchive.cpp

!IF  "$(CFG)" == "Vat - Win32 Release"

!ELSEIF  "$(CFG)" == "Vat - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vat - Win32 Release with debug symbols"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\engine\CreaturesArchive.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\File.cpp

!IF  "$(CFG)" == "Vat - Win32 Release"

!ELSEIF  "$(CFG)" == "Vat - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vat - Win32 Release with debug symbols"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\engine\File.h
# End Source File
# Begin Source File

SOURCE=..\..\common\FileFuncs.cpp

!IF  "$(CFG)" == "Vat - Win32 Release"

!ELSEIF  "$(CFG)" == "Vat - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vat - Win32 Release with debug symbols"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\FileFuncs.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\FlightRecorder.cpp

!IF  "$(CFG)" == "Vat - Win32 Release"

!ELSEIF  "$(CFG)" == "Vat - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vat - Win32 Release with debug symbols"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\engine\FlightRecorder.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\Maths.cpp

!IF  "$(CFG)" == "Vat - Win32 Release"

!ELSEIF  "$(CFG)" == "Vat - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vat - Win32 Release with debug symbols"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\engine\Maths.h
# End Source File
# Begin Source File

SOURCE=..\..\engine\PersistentObject.cpp

!IF  "$(CFG)" == "Vat - Win32 Release"

!ELSEIF  "$(CFG)" == "Vat - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vat - Win32 Release with debug symbols"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\engine\PersistentObject.h
# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\common\Catalogue.cpp

!IF  "$(CFG)" == "Vat - Win32 Release"

!ELSEIF  "$(CFG)" == "Vat - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vat - Win32 Release with debug symbols"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\Catalogue.h
# End Source File
# Begin Source File

SOURCE=..\..\common\clientside.cpp

!IF  "$(CFG)" == "Vat - Win32 Release"

!ELSEIF  "$(CFG)" == "Vat - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vat - Win32 Release with debug symbols"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\clientside.h
# End Source File
# Begin Source File

SOURCE=..\..\common\FileLocaliser.cpp

!IF  "$(CFG)" == "Vat - Win32 Release"

!ELSEIF  "$(CFG)" == "Vat - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vat - Win32 Release with debug symbols"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\FileLocaliser.h
# End Source File
# Begin Source File

SOURCE=..\..\common\GameInterface.cpp

!IF  "$(CFG)" == "Vat - Win32 Release"

!ELSEIF  "$(CFG)" == "Vat - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vat - Win32 Release with debug symbols"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\GameInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\common\Grapher.cpp

!IF  "$(CFG)" == "Vat - Win32 Release"

!ELSEIF  "$(CFG)" == "Vat - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vat - Win32 Release with debug symbols"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\Grapher.h
# End Source File
# Begin Source File

SOURCE=..\..\common\SimpleLexer.cpp

!IF  "$(CFG)" == "Vat - Win32 Release"

!ELSEIF  "$(CFG)" == "Vat - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vat - Win32 Release with debug symbols"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\SimpleLexer.h
# End Source File
# Begin Source File

SOURCE=..\..\common\WhichEngine.cpp

!IF  "$(CFG)" == "Vat - Win32 Release"

!ELSEIF  "$(CFG)" == "Vat - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vat - Win32 Release with debug symbols"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\WhichEngine.h
# End Source File
# End Group
# End Target
# End Project
