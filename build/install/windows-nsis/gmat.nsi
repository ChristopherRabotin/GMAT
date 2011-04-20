; GMAT NSIS installer script. This file is run by makesetup.sh, the
; top-level installer build script.

; Includes
!include FileFunc.nsh
!include LogicLib.nsh
!include MUI2.nsh

; General configuration
Name "GMAT R2011a"
OutFile "Setup.exe"
SetCompressor /SOLID lzma

; Multi-user configuration
!define MULTIUSER_EXECUTIONLEVEL Highest
!define MULTIUSER_MUI
!define MULTIUSER_INSTALLMODE_INSTDIR "GMAT\R2011a"
!define MULTIUSER_INSTALLMODE_COMMANDLINE
!include MultiUser.nsh

; User interface configuration
!define MUI_ICON "gmat\data\graphics\icons\GMAT.ico"
!define MUI_UNICON "gmat\data\graphics\icons\GMAT.ico"
!define MUI_DIRECTORYPAGE_VERIFYONLEAVE

; User interface pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MULTIUSER_PAGE_INSTALLMODE
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE InstDirCheck
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH
!insertmacro MUI_LANGUAGE "English"

; GMAT path settings
!define UNINST_KEY \
    "Software\Microsoft\Windows\CurrentVersion\Uninstall\GmatR2011a"

; Make not show UAC on Win7
RequestExecutionLevel user

; Installer
Section "GMAT"
    SetOutPath "$INSTDIR"
    File /r "gmat\*.*"
    
    ; Start Menu shortcut
    SetOutPath "$INSTDIR\bin"
    CreateDirectory "$SMPROGRAMS\GMAT"
    CreateShortCut "$SMPROGRAMS\GMAT\GMAT R2011a.lnk" "$INSTDIR\bin\GMAT.exe" "" "$INSTDIR\bin\GMAT.exe" 0
    
    ; Uninstaller and Add/Remove Programs registry key
    WriteUninstaller "$INSTDIR\Uninstall.exe"
    WriteRegStr SHCTX "${UNINST_KEY}" "DisplayName" "GMAT R2011a"
    WriteRegStr SHCTX "${UNINST_KEY}" "UninstallString" \
        "$\"$INSTDIR\uninstall.exe$\" /$MultiUser.InstallMode"
    WriteRegStr SHCTX "${UNINST_KEY}" "QuietUninstallString" \
        "$\"$INSTDIR\uninstall.exe$\" /$MultiUser.InstallMode /S"
    WriteRegStr SHCTX "${UNINST_KEY}" "DisplayVersion" "R2011a"
    WriteRegStr SHCTX "${UNINST_KEY}" "Publisher" "GMAT Development Team"
    WriteRegStr SHCTX "${UNINST_KEY}" "Version" "R2011a"
    WriteRegStr SHCTX "${UNINST_KEY}" "HelpLink" "http://gmat.ed-pages.com/forum/"
    WriteRegStr SHCTX "${UNINST_KEY}" "InstallLocation" "$\"$INSTDIR$\""
    WriteRegStr SHCTX "${UNINST_KEY}" "URLInfoAbout" "http://gmat.gsfc.nasa.gov/"
    WriteRegStr SHCTX "${UNINST_KEY}" "URLUpdateInfo" "http://sourceforge.net/projects/gmat/"
    WriteRegStr SHCTX "${UNINST_KEY}" "Comments" "General Mission Analysis Tool (GMAT)"
    WriteRegDWORD SHCTX "${UNINST_KEY}" "Language" "1033"
    WriteRegDWORD SHCTX "${UNINST_KEY}" "NoModify" "1"
    WriteRegDWORD SHCTX "${UNINST_KEY}" "NoRepair" "1"
    WriteRegStr SHCTX "${UNINST_KEY}" "Readme" "$\"$INSTDIR\README.txt$\""
    WriteRegStr SHCTX "${UNINST_KEY}" "DisplayIcon" "$\"$INSTDIR\data\graphics\icons\GMAT.ico$\""
    
    ; Estimated size
    ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
    IntFmt $0 "0x%08X" $0
    WriteRegDWORD SHCTX "${UNINST_KEY}" "EstimatedSize" "$0"
    
SectionEnd

; Uninstaller
Section "Uninstall"
    ; Uninstaller itself
    Delete "$INSTDIR\Uninstall.exe"
    DeleteRegKey SHCTX "${UNINST_KEY}"
    
    ; GMAT files
    !include .\uninstall.nsh
    
    ; Delete top-level GMAT folder if it exists
    IfFileExists "$INSTDIR\..\..\GMAT\*.*" +1 +2
        RMDir "$INSTDIR\..\..\GMAT"
    
    ; Start Menu shortcut
    Delete "$SMPROGRAMS\GMAT\GMAT R2011a.lnk"
    RMDir "$SMPROGRAMS\GMAT"
SectionEnd

Function InstDirCheck
    GetInstDirError $0
    ${Switch} $0
        ${Case} 0
            IfFileExists "$INSTDIR\*.*" +1 +3
                MessageBox MB_YESNO|MB_ICONINFORMATION|MB_DEFBUTTON2 "The directory $INSTDIR already exists. Do you want to install there anyway?" IDNO +1 IDYES +2
                Abort
            ${Break}
        ${Case} 1
            MessageBox MB_OK|MB_ICONSTOP "The directory $INSTDIR is invalid."
            Abort
            ${Break}
        ${Case} 2
            MessageBox MB_OK|MB_ICONSTOP "The is not enough disk space available to install."
            Abort
            ${Break}
    ${EndSwitch}
FunctionEnd

; Required by MultiUser.nsh
Function .onInit
  !insertmacro MULTIUSER_INIT
FunctionEnd

Function un.onInit
  !insertmacro MULTIUSER_UNINIT
FunctionEnd