; Define your application name
!define APPNAME "openc2e"
!define APPNAMEANDVERSION "openc2e (development build)"

; Development build revision
!define REVISION "1511"

; Main Install settings
Name "${APPNAMEANDVERSION}"
InstallDir "$PROGRAMFILES\openc2e"
InstallDirRegKey HKLM "Software\${APPNAME}" ""
OutFile "installers\openc2e-dev${REVISION}-win32.exe"

; Use compression
SetCompressor lzma

; Modern interface settings
!include "MUI.nsh"

; Icons
!define MUI_ICON "instal.ico"
!define MUI_UNICON "uninst.ico"

!define MUI_TEXT "LOL WUT"

!define MUI_ABORTWARNING

;Enable the start menu selection box
!define MUI_DIRECTORYPAGE

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "COPYING"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; Set languages (first is default language)
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Dutch"
!insertmacro MUI_LANGUAGE "French"
!insertmacro MUI_LANGUAGE "German"
!insertmacro MUI_LANGUAGE "Spanish"
!insertmacro MUI_RESERVEFILE_LANGDLL

!macro IfKeyExists ROOT MAIN_KEY KEY
push $R0
push $R1
 
!define Index 'Line${__LINE__}'
 
StrCpy $R1 "0"
 
"${Index}-Loop:"
; Check for Key
EnumRegKey $R0 ${ROOT} "${MAIN_KEY}" "$R1"
StrCmp $R0 "" "${Index}-False"
  IntOp $R1 $R1 + 1
  StrCmp $R0 "${KEY}" "${Index}-True" "${Index}-Loop"
 
"${Index}-True:"
;Return 1 if found
push "1"
goto "${Index}-End"
 
"${Index}-False:"
;Return 0 if not found
push "0"
goto "${Index}-End"
 
"${Index}-End:"
!undef Index
exch 2
pop $R0
pop $R1
!macroend

!macro GetCleanDir INPUTDIR
  ; ATTENTION: USE ON YOUR OWN RISK!
  ; Please report bugs here: http://stefan.bertels.org/
  !define Index_GetCleanDir 'GetCleanDir_Line${__LINE__}'
  Push $R0
  Push $R1
  StrCpy $R0 "${INPUTDIR}"
  StrCmp $R0 "" ${Index_GetCleanDir}-finish
  StrCpy $R1 "$R0" "" -1
  StrCmp "$R1" "\" ${Index_GetCleanDir}-finish
  StrCpy $R0 "$R0\"
${Index_GetCleanDir}-finish:
  Pop $R1
  Exch $R0
  !undef Index_GetCleanDir
!macroend

!macro RemoveFilesAndSubDirs DIRECTORY
  ; ATTENTION: USE ON YOUR OWN RISK!
  ; Please report bugs here: http://stefan.bertels.org/
  !define Index_RemoveFilesAndSubDirs 'RemoveFilesAndSubDirs_${__LINE__}'
 
  Push $R0
  Push $R1
  Push $R2
 
  !insertmacro GetCleanDir "${DIRECTORY}"
  Pop $R2
  FindFirst $R0 $R1 "$R2*.*"
${Index_RemoveFilesAndSubDirs}-loop:
  StrCmp $R1 "" ${Index_RemoveFilesAndSubDirs}-done
  StrCmp $R1 "." ${Index_RemoveFilesAndSubDirs}-next
  StrCmp $R1 ".." ${Index_RemoveFilesAndSubDirs}-next
  IfFileExists "$R2$R1\*.*" ${Index_RemoveFilesAndSubDirs}-directory
  ; file
  Delete "$R2$R1"
  goto ${Index_RemoveFilesAndSubDirs}-next
${Index_RemoveFilesAndSubDirs}-directory:
  ; directory
  RMDir /r "$R2$R1"
${Index_RemoveFilesAndSubDirs}-next:
  FindNext $R0 $R1
  Goto ${Index_RemoveFilesAndSubDirs}-loop
${Index_RemoveFilesAndSubDirs}-done:
  FindClose $R0
 
  Pop $R2
  Pop $R1
  Pop $R0
  !undef Index_RemoveFilesAndSubDirs
!macroend

Function CheckVCRedist
   Push $R0
   ClearErrors
   ReadRegDword $R0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{7299052b-02a4-4627-81f2-1818da5d550d}" "Version"

   ; if VS 2005+ redist SP1 not installed, install it
   IfErrors 0 yay
   StrCpy $R0 "-1"
	 Goto done

yay:
	StrCpy $R0 "1"

done:
	Exch $R0
FunctionEnd

Section "openc2e" Main

	; Set Section properties
	SetOverwrite on

	; Set Section Files and Shortcuts
	SetOutPath "$INSTDIR\"
	
	IfFileExists "$INSTDIR\uninstall.exe" remold
	Goto proceed
	
	remold:
	MessageBox MB_YESNO 'An old version of openc2e is installed.  Do you wish to$\rremove it before proceeding (recommended)?' IDYES remold1
	Goto proceed
	remold1:
	ExecWait '"$INSTDIR\uninstall.exe" _?=$INSTDIR /S'
	
	proceed:
	File "Release\openc2e.exe"
	File "Release\SDL.dll"
	File "Release\SDL_mixer.dll"
	File "Release\SDL_net.dll"
	File "Release\zlib1.dll"
	File "Release\QtCore4.dll"
	File "Release\QtGui4.dll"
	File "Release\QtNetwork4.dll"
	File "Release\alut.dll"
	
	;OpenAL installer
	File "Release\oalinst.exe"
	
	File "tools\braininavat\Brain-in-a-vat\Release\Brain-in-a-vat.exe"
	File "tools\debugkit\DebugKit\release\DebugKit.exe"
	File "gamefinder.exe"
	
	CreateDirectory "$SMPROGRAMS\openc2e\Tools"
	CreateDirectory "$SMPROGRAMS\openc2e\Games"
	CreateDirectory "$SMPROGRAMS\openc2e"
	CreateShortCut "$SMPROGRAMS\openc2e\Tools\Brain-in-a-Vat.lnk" "$INSTDIR\Brain-in-a-vat.exe"
	CreateShortCut "$SMPROGRAMS\openc2e\Tools\Debug Kit.lnk" "$INSTDIR\DebugKit.exe"
	CreateShortCut "$SMPROGRAMS\openc2e\Update Game Shortcuts.lnk" "$INSTDIR\gamefinder.exe"
	CreateShortCut "$SMPROGRAMS\openc2e\Uninstall.lnk" "$INSTDIR\uninstall.exe"
	
SectionEnd

Section -FinishSection

	WriteRegStr HKLM "Software\${APPNAME}" "" "$INSTDIR"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$INSTDIR\uninstall.exe"
	WriteUninstaller "$INSTDIR\uninstall.exe"
	
	;Run the OpenAL installer silently
	DetailPrint "Installing OpenAL sound library..."
	ExecWait '"$INSTDIR\oalinst.exe" /s'
	Delete "$INSTDIR\oalinst.exe"
	
	DetailPrint "Checking for VC++ runtime libraries..."
	Call CheckVCRedist
	Pop $R0
	StrCmp $R0 "1" foundvc
	ReadRegDword $R0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{7299052b-02a4-4627-81f2-1818da5d550d}" "Version"
	MessageBox MB_YESNO "Based on a cursory examination, you do not appear to have necessary VC++ runtime$\rlibraries installed.  However, if you have Visual Studio 2005 installed, I could$\rbe wrong.  Would you like to download and install these libraries? (2.5mb or so)$\rIf you do not know for sure, I'd go with 'Yes'." IDYES installvc
	MessageBox MB_OK "Fair enough.  If openc2e fails to run, make sure to acquire the Visual C++ 2005$\rruntime libraries from http://openc2e.ccdevnet.org/files/win32/vcredist_x86.exe"
	Goto finishup
	
	foundvc:
	DetailPrint "VC++ runtime libraries found (probably.)"
	Goto finishup
	
	installvc:
	DetailPrint "Downloading VC++ runtime library redistributable..."
	NSISdl::download http://openc2e.ccdevnet.org/files/win32/vcredist_x86.exe $INSTDIR\vcredist_x86.exe
	Pop $R0
	StrCmp $R0 "success" runvcinst
	MessageBox MB_OK|MB_ICONSTOP "The download of the VC++ runtime library installer failed: $R0   $\rSorry, but I guess you'll have to download it yourself. :($\r (http://openc2e.ccdevnet.org/files/win32/vcredist_x86.exe)"
	Goto finishup
	runvcinst:
	ExecWait "$INSTDIR\vcredist_x86.exe"
	Delete "$INSTDIR\vcredist_x86.exe"
	
	finishup:
	;Run the shortcut-maker
	MessageBox MB_YESNO 'Look for installed Creatures games now? $\rIf you choose to wait, you can run "Update Game Shortcuts" from the start menu later.' IDYES run
	Goto end
	run:
	DetailPrint "Searching for installed creatures games..."
	ExecWait "$INSTDIR\gamefinder.exe"
	end:
SectionEnd

; Modern install component descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${Main} ""
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;Uninstall section
Section Uninstall

	; Remove from registry...
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
	DeleteRegKey HKLM "SOFTWARE\${APPNAME}"
	
	; Delete Program Files
	!insertmacro RemoveFilesAndSubDirs "$INSTDIR\"

	; Delete Shortcuts
	!insertmacro RemoveFilesAndSubDirs "$SMPROGRAMS\${APPNAME}\"

	; Remove remaining directories
	RMDir "$SMPROGRAMS\openc2e\Tools"
	RMDir "$SMPROGRAMS\openc2e\Games"
	RMDir "$SMPROGRAMS\openc2e"
	RMDir "$INSTDIR\"

SectionEnd

; On initialization
Function .onInit

	!insertmacro MUI_LANGDLL_DISPLAY

FunctionEnd

BrandingText "http://openc2e.ccdevnet.org"