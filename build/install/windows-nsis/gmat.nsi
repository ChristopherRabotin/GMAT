; Includes
!include MUI2.nsh

; General configuration
Name "GMAT"
OutFile "Setup.exe"
SetCompressor lzma

; Multi-user configuration
!define MULTIUSER_EXECUTIONLEVEL Highest
!define MULTIUSER_MUI
!define MULTIUSER_INSTALLMODE_INSTDIR "GMAT\R2011a"
!define MULTIUSER_INSTALLMODE_COMMANDLINE
!include MultiUser.nsh

; User interface
!insertmacro MUI_PAGE_WELCOME
!insertmacro MULTIUSER_PAGE_INSTALLMODE
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
    CreateDirectory "$SMPROGRAMS\GMAT\R2011a"
    CreateShortCut "$SMPROGRAMS\GMAT\R2011a\GMAT.lnk" "$INSTDIR\bin\GMAT.exe" "" "$INSTDIR\bin\GMAT.exe" 0
    
    ; Uninstaller
    WriteUninstaller "$INSTDIR\Uninstall.exe"
    WriteRegStr SHCTX "${UNINST_KEY}" "DisplayName" "GMAT R2011a"
    WriteRegStr SHCTX "${UNINST_KEY}" "UninstallString" \
        "$\"$INSTDIR\uninstall.exe$\" /$MultiUser.InstallMode"
    WriteRegStr SHCTX "${UNINST_KEY}" "QuietUninstallString" \
        "$\"$INSTDIR\uninstall.exe$\" /$MultiUser.InstallMode /S"
SectionEnd

; Uninstaller
Section "Uninstall"
    ; Uninstaller itself
    Delete "$INSTDIR\Uninstall.exe"
    DeleteRegKey SHCTX "${UNINST_KEY}"
    
    ; GMAT files
    !include .\uninstall.nsh
    
    ; TODO: delete GMAT folder if it exists
    
    ; Start Menu shortcut
    Delete "$SMPROGRAMS\GMAT\R2011a\GMAT.lnk"
    RMDir "$SMPROGRAMS\GMAT\R2011a"
    RMDir "$SMPROGRAMS\GMAT"
SectionEnd

; Required by MultiUser.nsh
Function .onInit
  !insertmacro MULTIUSER_INIT
FunctionEnd

Function un.onInit
  !insertmacro MULTIUSER_UNINIT
FunctionEnd