; Openc2e Game Finder Script
; by Eric Goodwin (ElasticMuffin)
; 4/29/06
; Last Update: 10/24/06 (add C2)


Name "openc2e Game Finder Script"
OutFile "gamefinder.exe"
ShowInstDetails show

SetCompressor lzma

Icon "gamefind.ico"

!include LogicLib.nsh

Var dir
Var plainc3
Var plainc3exist
Var plainds
Var edynn
Var exodc3
Var exodc3exist
Var exodds
Var ca
Var cp
Var cv
Var plainc2
Var plainc1
Var tayc2
Var tayc1

;RegKey checker
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

Section
	
  ReadRegStr $dir HKLM "Software\openc2e" ""
	
	DetailPrint "Checking for Creatures games installed on your computer..."
	DetailPrint ""

  ; Vanilla C3
	!insertmacro IfKeyExists "HKEY_LOCAL_MACHINE" "Software\CyberLife Technology" "Creatures 3"
	Pop $R0
	${If} $R0 = 1
	  DetailPrint "Found Creatures 3 (original)."
		ReadRegStr $plainc3 HKEY_LOCAL_MACHINE "Software\CyberLife Technology\Creatures 3" "Main Directory"
		StrCpy $R1 "$plainc3" 1 -1
		StrCpy $plainc3exist "1"
		StrCmp "$R1" "\" trim1
		Goto make1
		trim1:
		StrCpy $plainc3 "$plainc3" -1
		make1:
		CreateShortCut "$SMPROGRAMS\openc2e\Games\Creatures 3.lnk" "$dir\openc2e.exe" '-d "$plainc3" --autokill'
	${Else}
	  DetailPrint "Could not find Creatures 3 (original)."
		Delete "$SMPROGRAMS\openc2e\Games\Creatures 3.lnk"
	${EndIf}

	; Vanilla DS
	!insertmacro IfKeyExists "HKEY_LOCAL_MACHINE" "Software\CyberLife Technology" "Docking Station"
	Pop $R0
	${If} $R0 = 1
	  DetailPrint "Found Docking Station (original)."
		ReadRegStr $plainds HKEY_LOCAL_MACHINE "Software\CyberLife Technology\Docking Station" "Main Directory"
		StrCpy $R1 "$plainds" 1 -1
		StrCmp "$R1" "\" trim2
		Goto make2
		trim2:
		StrCpy $plainds "$plainds" -1
		make2:
		CreateShortCut "$SMPROGRAMS\openc2e\Games\Docking Station.lnk" "$dir\openc2e.exe" '--gamename "Docking Station" -d "$plainds" --autokill'
		${If} $plainc3exist == "1"
			DetailPrint "Creating Docked shortcut for C3/DS (original)."
		  CreateShortCut "$SMPROGRAMS\openc2e\Games\C3 + DS (Docked).lnk" "$dir\openc2e.exe" '--gamename "Docking Station" -d "$plainc3" -d "$plainds" --autokill'
		${Else}
		  Delete "$SMPROGRAMS\openc2e\Games\C3 + DS (Docked).lnk"
		${EndIf}
	${Else}
		DetailPrint "Could not find Docking Station (original)."
	  Delete "$SMPROGRAMS\openc2e\Docking Station.lnk"
	${EndIf}

	; Edynn
  !insertmacro IfKeyExists "HKEY_LOCAL_MACHINE" "Software\CyberLife Technology" "Edynn"
	Pop $R0
	${If} $R0 = 1
	  DetailPrint "Found Edynn."
		ReadRegStr $edynn HKEY_LOCAL_MACHINE "Software\CyberLife Technology\Edynn" "Main Directory"
		StrCpy $R1 "$edynn" 1 -1
		StrCmp "$R1" "\" trim3
		Goto make3
		trim3:
		StrCpy $edynn "$edynn" -1
		make3:
		CreateShortCut "$SMPROGRAMS\openc2e\Games\Edynn.lnk" "$dir\openc2e.exe" '--gamename "Edynn" -d "$edynn" --autokill'
	${Else}
	  DetailPrint "Could not find Edynn."
	  Delete "$SMPROGRAMS\openc2e\Games\Edynn.lnk"
	${EndIf}

	;Exodus C3
	!insertmacro IfKeyExists "HKEY_LOCAL_MACHINE" "Software\Gameware Development" "Creatures 3"
	Pop $R0
	${If} $R0 = 1
	  DetailPrint "Found Creatures 3 (Exodus)."
		ReadRegStr $exodc3 HKEY_LOCAL_MACHINE "Software\Gameware Development\Creatures 3" "Main Directory"
		StrCpy $R1 "$exodc3" 1 -1
		StrCpy $exodc3exist "1"
		StrCmp "$R1" "\" trim4
		Goto make4
		trim4:
		StrCpy $exodc3 "$exodc3" -1
		make4:
		CreateShortCut "$SMPROGRAMS\openc2e\Games\Creatures 3 (Exodus).lnk" "$dir\openc2e.exe" '-d "$exodc3" --autokill'
	${Else}
	  DetailPrint "Could not find Creatures 3 (Exodus)."
	  Delete "$SMPROGRAMS\openc2e\Games\Creatures 3 (Exodus).lnk"
	${EndIf}

	;Exodus DS
	!insertmacro IfKeyExists "HKEY_LOCAL_MACHINE" "Software\Gameware Development" "Docking Station"
	Pop $R0
	${If} $R0 = 1
	  DetailPrint "Found Docking Station (Exodus)."
		ReadRegStr $exodds HKEY_LOCAL_MACHINE "Software\Gameware Development\Docking Station" "Main Directory"
		StrCpy $R1 "$exodds" 1 -1
		StrCmp "$R1" "\" trim5
		Goto make5
		trim5:
		StrCpy $exodds "$exodds" -1
		make5:
		CreateShortCut "$SMPROGRAMS\openc2e\Games\Docking Station (Exodus).lnk" "$dir\openc2e.exe" '--gamename "Docking Station" -d "$exodds" --autokill'
		${If} $exodc3exist == "1"
			DetailPrint "Creating Docked shortcut for C3/DS (Exodus)."
		  CreateShortCut "$SMPROGRAMS\openc2e\Games\C3 + DS Exodus (Docked).lnk" "$dir\openc2e.exe" '--gamename "Docking Station" -d "$plainc3" -d "$plainds" --autokill'
		${Else}
		  Delete "$SMPROGRAMS\openc2e\Games\C3 + DS Exodus (Docked).lnk"
		${EndIf}
	${Else}
	  DetailPrint "Could not find Docking Station (Exodus)."
	  Delete "$SMPROGRAMS\openc2e\Games\Docking Station (Exodus).lnk"
	${EndIf}
	
	;Creatures Adventures
  !insertmacro IfKeyExists "HKEY_LOCAL_MACHINE" "Software\CyberLife Technology" "Creatures Adventures"
	Pop $R0
	${If} $R0 = 1
	  DetailPrint "Found Creatures Adventures."
		ReadRegStr $ca HKEY_LOCAL_MACHINE "Software\CyberLife Technology\Creatures Adventures" "Main Directory"
		StrCpy $R1 "$ca" 1 -1
		StrCmp "$R1" "\" trim6
		Goto make6
		trim6:
		StrCpy $ca "$ca" -1
		make6:
		CreateShortCut "$SMPROGRAMS\openc2e\Games\Creatures Adventures.lnk" "$dir\openc2e.exe" '--gamename "Creatures Adventures" -d "$ca" -g cv'
	${Else}
	  DetailPrint "Could not find Creatures Adventures."
	  Delete "$SMPROGRAMS\openc2e\Games\Creatures Adventures.lnk"
	${EndIf}
	
	;Creatures Playground
  !insertmacro IfKeyExists "HKEY_LOCAL_MACHINE" "Software\CyberLife Technology" "Creatures Playground"
	Pop $R0
	${If} $R0 = 1
	  DetailPrint "Found Creatures Playground."
		ReadRegStr $cp HKEY_LOCAL_MACHINE "Software\CyberLife Technology\Creatures Playground" "Main Directory"
		StrCpy $R1 "$cp" 1 -1
		StrCmp "$R1" "\" trim7
		Goto make7
		trim7:
		StrCpy $cp "$cp" -1
		make7:
		CreateShortCut "$SMPROGRAMS\openc2e\Games\Creatures Playground.lnk" "$dir\openc2e.exe" '--gamename "Creatures Playground" -d "$cp" -g cv'
	${Else}
	  DetailPrint "Could not find Creatures Playground."
	  Delete "$SMPROGRAMS\openc2e\Games\Creatures Playground.lnk"
	${EndIf}
	
	;Creatures Village
	!insertmacro IfKeyExists "HKEY_LOCAL_MACHINE" "Software\Gameware Development" "Creatures Village"
	Pop $R0
	${If} $R0 = 1
	  DetailPrint "Found Creatures Village."
		ReadRegStr $cv HKEY_LOCAL_MACHINE "Software\Gameware Development\Creatures Village" "Main Directory"
		StrCpy $R1 "$cv" 1 -1
		StrCmp "$R1" "\" trim8
		Goto make8
		trim8:
		StrCpy $cv "$cv" -1
		make8:
		CreateShortCut "$SMPROGRAMS\openc2e\Games\Creatures Village.lnk" "$dir\openc2e.exe" '-d "$cv" -g cv'
	${Else}
	  DetailPrint "Could not find Creatures Village."
	  Delete "$SMPROGRAMS\openc2e\Games\Creatures Village.lnk"
	${EndIf}
	
	;Original C2
	!insertmacro IfKeyExists "HKEY_LOCAL_MACHINE" "Software\Cyberlife Technology" "Creatures 2"
	Pop $R0
	${If} $R0 = 1
	  DetailPrint "Found Creatures 2 (original)."
		ReadRegStr $plainc2 HKEY_LOCAL_MACHINE "Software\Cyberlife Technology\Creatures 2\1.0" "Main Directory"
		StrCpy $R1 "$plainc2" 1 -1
		StrCmp "$R1" "\" trim9
		Goto make9
		trim9:
		StrCpy $plainc2 "$plainc2" -1
		make9:
		CreateShortCut "$SMPROGRAMS\openc2e\Games\Creatures 2.lnk" "$dir\openc2e.exe" '-d "$plainc2" -g c2 --autokill'
	${Else}
	  DetailPrint "Could not find Creatures 2 (original)."
	  Delete "$SMPROGRAMS\openc2e\Games\Creatures 2.lnk"
	${EndIf}
	
		;Original C1
	!insertmacro IfKeyExists "HKEY_LOCAL_MACHINE" "Software\Millennium Interactive" "Creatures"
	Pop $R0
	${If} $R0 = 1
	  DetailPrint "Found Creatures 1 (original)."
		ReadRegStr $plainc1 HKEY_LOCAL_MACHINE "Software\Millennium Interactive\Creatures\1.0" "Main Directory"
		StrCpy $R1 "$plainc1" 1 -1
		StrCmp "$R1" "\" trim10
		Goto make10
		trim10:
		StrCpy $plainc1 "$plainc1" -1
		make10:
		CreateShortCut "$SMPROGRAMS\openc2e\Games\Creatures 1.lnk" "$dir\openc2e.exe" '-d "$plainc1" -g c1 --autokill'
	${Else}
	  DetailPrint "Could not find Creatures 1 (original)."
	  Delete "$SMPROGRAMS\openc2e\Games\Creatures 1.lnk"
	${EndIf}
	
		;TAY C2
	!insertmacro IfKeyExists "HKEY_LOCAL_MACHINE" "Software\Gameware Development" "Creatures 2"
	Pop $R0
	${If} $R0 = 1
	  DetailPrint "Found Creatures 2 (The Albian Years)."
		ReadRegStr $tayc2 HKEY_LOCAL_MACHINE "Software\Gameware Development\Creatures 2\1.0" "Main Directory"
		StrCpy $R1 "$tayc2" 1 -1
		StrCmp "$R1" "\" trim11
		Goto make11
		trim11:
		StrCpy $tayc2 "$tayc2" -1
		make11:
		CreateShortCut "$SMPROGRAMS\openc2e\Games\Creatures 2 (TAY).lnk" "$dir\openc2e.exe" '-d "$tayc2" -g c2 --autokill'
	${Else}
	  DetailPrint "Could not find Creatures 2 (The Albian Years)."
	  Delete "$SMPROGRAMS\openc2e\Games\Creatures 2 (TAY).lnk"
	${EndIf}
	
		;TAY C1
	!insertmacro IfKeyExists "HKEY_LOCAL_MACHINE" "Software\Gameware Development" "Creatures 1"
	Pop $R0
	${If} $R0 = 1
	  DetailPrint "Found Creatures 1 (The Albian Years)."
		ReadRegStr $tayc1 HKEY_LOCAL_MACHINE "Software\Gameware Development\Creatures 1\1.0" "Main Directory"
		StrCpy $R1 "$tayc1" 1 -1
		StrCmp "$R1" "\" trim12
		Goto make12
		trim12:
		StrCpy $tayc1 "$tayc1" -1
		make12:
		CreateShortCut "$SMPROGRAMS\openc2e\Games\Creatures 1 (TAY).lnk" "$dir\openc2e.exe" '-d "$tayc1" -g c1 --autokill'
	${Else}
	  DetailPrint "Could not find Creatures 1 (The Albian Years)."
	  Delete "$SMPROGRAMS\openc2e\Games\Creatures 1 (TAY).lnk"
	${EndIf}
SectionEnd

BrandingText "openc2e Gamefinder"