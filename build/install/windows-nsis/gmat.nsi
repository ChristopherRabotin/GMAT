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
!define SRCDIR "..\trunk\application"
!define UNINST_KEY \
    "Software\Microsoft\Windows\CurrentVersion\Uninstall\GmatR2011a"

; Make not show UAC on Win7
RequestExecutionLevel user

; Installer
Section "GMAT"
    ; root
    SetOutPath "$INSTDIR"

    ; bin
    SetOutPath "$INSTDIR\bin"
    File "${SRCDIR}\bin\GMAT.exe"
    File "${SRCDIR}\bin\GMAT.ini"
    File "${SRCDIR}\bin\gmat_startup_file.txt"
    File "${SRCDIR}\bin\libpcre-0.dll"
    File "${SRCDIR}\bin\libpcrecpp-0.dll"
    File "${SRCDIR}\bin\mingwm10.dll"
    File "${SRCDIR}\bin\wxbase28_gcc_custom.dll"
    File "${SRCDIR}\bin\wxmsw28_adv_gcc_custom.dll"
    File "${SRCDIR}\bin\wxmsw28_core_gcc_custom.dll"
    File "${SRCDIR}\bin\wxmsw28_gl_gcc_custom.dll"
    File "${SRCDIR}\bin\wxmsw28_netutils_gcc_custom.dll"
    File "${SRCDIR}\bin\wxmsw28_stc_gcc_custom.dll"
    
    ; data\graphics\icons
    SetOutPath "$INSTDIR\data\graphics\icons"
    File "${SRCDIR}\data\graphics\icons\CloseAll.png"
    File "${SRCDIR}\data\graphics\icons\CloseOne.png"
    File "${SRCDIR}\data\graphics\icons\Copy.png"
    File "${SRCDIR}\data\graphics\icons\Cut.png"
    File "${SRCDIR}\data\graphics\icons\FasterAnimation.png"
    File "${SRCDIR}\data\graphics\icons\GMAT.ico"
    File "${SRCDIR}\data\graphics\icons\GMATAboutDialog.jpg"
    File "${SRCDIR}\data\graphics\icons\GMATAboutIcon.jpg"
    File "${SRCDIR}\data\graphics\icons\GMATIcon.icns"
    File "${SRCDIR}\data\graphics\icons\GMATIcon.jpg"
    File "${SRCDIR}\data\graphics\icons\GMATIcon.pic"
    File "${SRCDIR}\data\graphics\icons\GMATIcon.psd"
    File "${SRCDIR}\data\graphics\icons\GMATLinux48.xpm"
    File "${SRCDIR}\data\graphics\icons\GMATLogo.jpg"
    File "${SRCDIR}\data\graphics\icons\GMATWin32.ico"
    File "${SRCDIR}\data\graphics\icons\Help.png"
    File "${SRCDIR}\data\graphics\icons\NewMission.png"
    File "${SRCDIR}\data\graphics\icons\NewScript.png"
    File "${SRCDIR}\data\graphics\icons\OpenScript.png"
    File "${SRCDIR}\data\graphics\icons\Paste.png"
    File "${SRCDIR}\data\graphics\icons\PauseMission.png"
    File "${SRCDIR}\data\graphics\icons\RunAnimation.png"
    File "${SRCDIR}\data\graphics\icons\RunMission.png"
    File "${SRCDIR}\data\graphics\icons\SaveMission.png"
    File "${SRCDIR}\data\graphics\icons\Screenshot.png"
    File "${SRCDIR}\data\graphics\icons\SlowerAnimation.png"
    File "${SRCDIR}\data\graphics\icons\StepByStep.jpg"
    File "${SRCDIR}\data\graphics\icons\StepByStep.psd"
    File "${SRCDIR}\data\graphics\icons\StopAnimation.png"
    File "${SRCDIR}\data\graphics\icons\StopMission.png"
    File "${SRCDIR}\data\graphics\icons\VideoTutorial.jpg"
    File "${SRCDIR}\data\graphics\icons\VideoTutorial.psd"
    File "${SRCDIR}\data\graphics\icons\WebHelp.png"
    
    ; data\graphics\splash
    SetOutPath "$INSTDIR\data\graphics\splash"
    File "${SRCDIR}\data\graphics\splash\GMATSplashScreen.tif"
    
    ; data\graphics\stars
    SetOutPath "$INSTDIR\data\graphics\stars"
    File "${SRCDIR}\data\graphics\stars\inp_Constellation.txt"
    File "${SRCDIR}\data\graphics\stars\inp_StarCatalog.txt"
    
    ; data\graphics\texture
    SetOutPath "$INSTDIR\data\graphics\texture"
    File "${SRCDIR}\data\graphics\texture\GenericCelestialBody.jpg"
    File "${SRCDIR}\data\graphics\texture\Jupiter_HermesCelestiaMotherlode.jpg"
    File "${SRCDIR}\data\graphics\texture\Mars_JPLCaltechUSGS.jpg"
    File "${SRCDIR}\data\graphics\texture\Mercury_JPLCaltech.jpg"
    File "${SRCDIR}\data\graphics\texture\ModifiedBlueMarble.jpg"
    File "${SRCDIR}\data\graphics\texture\Moon_HermesCelestiaMotherlode.jpg"
    File "${SRCDIR}\data\graphics\texture\Neptune_BjornJonsson.jpg"
    File "${SRCDIR}\data\graphics\texture\Pluto_JPLCaltech.jpg"
    File "${SRCDIR}\data\graphics\texture\Saturn_gradiusCelestiaMotherlode.jpg"
    File "${SRCDIR}\data\graphics\texture\Sun.jpg"
    File "${SRCDIR}\data\graphics\texture\Uranus_JPLCaltech.jpg"
    File "${SRCDIR}\data\graphics\texture\Venus_BjornJonsson.jpg"
    
    ; data\gravity\earth
    SetOutPath "$INSTDIR\data\gravity\earth"
    File "${SRCDIR}\data\gravity\earth\EGM96.cof"
    File "${SRCDIR}\data\gravity\earth\EGM96low.cof"
    File "${SRCDIR}\data\gravity\earth\JGM2.cof"
    File "${SRCDIR}\data\gravity\earth\JGM2F70.cof"
    File "${SRCDIR}\data\gravity\earth\JGM3.cof"
    
    ; data\gravity\luna
    SetOutPath "$INSTDIR\data\gravity\luna"
    File "${SRCDIR}\data\gravity\luna\LP165P.cof"
    
    ; data\gravity\mars
    SetOutPath "$INSTDIR\data\gravity\mars"
    File "${SRCDIR}\data\gravity\mars\GMM1.cof"
    File "${SRCDIR}\data\gravity\mars\GMM2B.cof"
    File "${SRCDIR}\data\gravity\mars\Mars50c.cof"
    
    ; data\gravity\venus
    SetOutPath "$INSTDIR\data\gravity\venus"
    File "${SRCDIR}\data\gravity\venus\MGN75HSAAP.cof"
    File "${SRCDIR}\data\gravity\venus\MGNP180U.cof"
    
    ; data\gui_config
    SetOutPath "$INSTDIR\data\gui_config"
    File "${SRCDIR}\data\gui_config\Antenna.ini"
    File "${SRCDIR}\data\gui_config\FuelTank.ini"
    File "${SRCDIR}\data\gui_config\Receiver.ini"
    File "${SRCDIR}\data\gui_config\Transmitter.ini"
    File "${SRCDIR}\data\gui_config\Transponder.ini"
    File "${SRCDIR}\data\gui_config\VF13ad.ini"
    
    ; data\IonosphereData
    SetOutPath "$INSTDIR\data\IonosphereData"
    File "${SRCDIR}\data\IonosphereData\ap.dat"
    File "${SRCDIR}\data\IonosphereData\ccir11.asc"
    File "${SRCDIR}\data\IonosphereData\ccir12.asc"
    File "${SRCDIR}\data\IonosphereData\ccir13.asc"
    File "${SRCDIR}\data\IonosphereData\ccir14.asc"
    File "${SRCDIR}\data\IonosphereData\ccir15.asc"
    File "${SRCDIR}\data\IonosphereData\ccir16.asc"
    File "${SRCDIR}\data\IonosphereData\ccir17.asc"
    File "${SRCDIR}\data\IonosphereData\ccir18.asc"
    File "${SRCDIR}\data\IonosphereData\ccir19.asc"
    File "${SRCDIR}\data\IonosphereData\ccir20.asc"
    File "${SRCDIR}\data\IonosphereData\ccir21.asc"
    File "${SRCDIR}\data\IonosphereData\ccir22.asc"
    File "${SRCDIR}\data\IonosphereData\dgrf00.dat"
    File "${SRCDIR}\data\IonosphereData\dgrf45.dat"
    File "${SRCDIR}\data\IonosphereData\dgrf50.dat"
    File "${SRCDIR}\data\IonosphereData\dgrf55.dat"
    File "${SRCDIR}\data\IonosphereData\dgrf60.dat"
    File "${SRCDIR}\data\IonosphereData\dgrf65.dat"
    File "${SRCDIR}\data\IonosphereData\dgrf70.dat"
    File "${SRCDIR}\data\IonosphereData\dgrf75.dat"
    File "${SRCDIR}\data\IonosphereData\dgrf80.dat"
    File "${SRCDIR}\data\IonosphereData\dgrf85.dat"
    File "${SRCDIR}\data\IonosphereData\dgrf90.dat"
    File "${SRCDIR}\data\IonosphereData\dgrf95.dat"
    File "${SRCDIR}\data\IonosphereData\igrf05.dat"
    File "${SRCDIR}\data\IonosphereData\igrf05s.dat"
    File "${SRCDIR}\data\IonosphereData\ig_rz.dat"
    File "${SRCDIR}\data\IonosphereData\ursi11.asc"
    File "${SRCDIR}\data\IonosphereData\ursi12.asc"
    File "${SRCDIR}\data\IonosphereData\ursi13.asc"
    File "${SRCDIR}\data\IonosphereData\ursi14.asc"
    File "${SRCDIR}\data\IonosphereData\ursi15.asc"
    File "${SRCDIR}\data\IonosphereData\ursi16.asc"
    File "${SRCDIR}\data\IonosphereData\ursi17.asc"
    File "${SRCDIR}\data\IonosphereData\ursi18.asc"
    File "${SRCDIR}\data\IonosphereData\ursi19.asc"
    File "${SRCDIR}\data\IonosphereData\ursi20.asc"
    File "${SRCDIR}\data\IonosphereData\ursi21.asc"
    File "${SRCDIR}\data\IonosphereData\ursi22.asc"
    
    ; data\planetary_coeff
    SetOutPath "$INSTDIR\data\planetary_coeff"
    File "${SRCDIR}\data\planetary_coeff\eopc04_08.62-now"
    File "${SRCDIR}\data\planetary_coeff\NUT85.DAT"
    File "${SRCDIR}\data\planetary_coeff\NUTATION.DAT"
    
    ; data\planetary_ephem\de
    SetOutPath "$INSTDIR\data\planetary_ephem\de"
    File "${SRCDIR}\data\planetary_ephem\de\leDE1941.405"
    
    ; data\planetary_ephem\spk
    SetOutPath "$INSTDIR\data\planetary_ephem\spk"
    File "${SRCDIR}\data\planetary_ephem\spk\ceres_1900_2100.bsp"
    File "${SRCDIR}\data\planetary_ephem\spk\de421.bsp"
    
    ; data\time
    SetOutPath "$INSTDIR\data\time"
    File "${SRCDIR}\data\time\naif0009.tls"
    File "${SRCDIR}\data\time\tai-utc.dat"
    
    ; data\vehicle\ephem
    SetOutPath "$INSTDIR\data\vehicle\ephem"
    
    ; data\vehicle\models
    SetOutPath "$INSTDIR\data\vehicle\models"
    File "${SRCDIR}\data\vehicle\models\aura.3ds"
    File "${SRCDIR}\data\vehicle\models\aura_map.jpg"
    
    ; matlab
    SetOutPath "$INSTDIR\matlab"
    File "${SRCDIR}\matlab\gmat_startup.m"
    
    ; matlab\gmat_command
    SetOutPath "$INSTDIR\matlab\gmat_command"
    File "${SRCDIR}\matlab\gmat_command\Achieve.m"
    File "${SRCDIR}\matlab\gmat_command\Assignment.m"
    File "${SRCDIR}\matlab\gmat_command\BeginFiniteBurn.m"
    File "${SRCDIR}\matlab\gmat_command\BeginScript.m"
    File "${SRCDIR}\matlab\gmat_command\BuildGMAT.m"
    File "${SRCDIR}\matlab\gmat_command\Case.m"
    File "${SRCDIR}\matlab\gmat_command\Create.m"
    File "${SRCDIR}\matlab\gmat_command\Else.m"
    File "${SRCDIR}\matlab\gmat_command\ElseIf.m"
    File "${SRCDIR}\matlab\gmat_command\EndFiniteBurn.m"
    File "${SRCDIR}\matlab\gmat_command\EndFor.m"
    File "${SRCDIR}\matlab\gmat_command\EndIf.m"
    File "${SRCDIR}\matlab\gmat_command\EndOptimize.m"
    File "${SRCDIR}\matlab\gmat_command\EndScript.m"
    File "${SRCDIR}\matlab\gmat_command\EndTarget.m"
    File "${SRCDIR}\matlab\gmat_command\EndWhile.m"
    File "${SRCDIR}\matlab\gmat_command\For.m"
    File "${SRCDIR}\matlab\gmat_command\GMAT.m"
    File "${SRCDIR}\matlab\gmat_command\If.m"
    File "${SRCDIR}\matlab\gmat_command\Maneuver.m"
    File "${SRCDIR}\matlab\gmat_command\Minimize.m"
    File "${SRCDIR}\matlab\gmat_command\NonLinearConstraint.m"
    File "${SRCDIR}\matlab\gmat_command\Optimize.m"
    File "${SRCDIR}\matlab\gmat_command\Propagate.m"
    File "${SRCDIR}\matlab\gmat_command\Report.m"
    File "${SRCDIR}\matlab\gmat_command\Save.m"
    File "${SRCDIR}\matlab\gmat_command\SBInput2Str.m"
    File "${SRCDIR}\matlab\gmat_command\SendGMATObject.m"
    File "${SRCDIR}\matlab\gmat_command\Stop.m"
    File "${SRCDIR}\matlab\gmat_command\Struct2Str.m"
    File "${SRCDIR}\matlab\gmat_command\Stuct2Str.m"
    File "${SRCDIR}\matlab\gmat_command\Target.m"
    File "${SRCDIR}\matlab\gmat_command\TestStruct2Str.m"
    File "${SRCDIR}\matlab\gmat_command\Toggle.m"
    File "${SRCDIR}\matlab\gmat_command\Vary.m"
    File "${SRCDIR}\matlab\gmat_command\While.m"
    
    ; matlab\gmat_fmincon
    SetOutPath "$INSTDIR\matlab\gmat_fmincon"
    File "${SRCDIR}\matlab\gmat_fmincon\CallGMATfminconSolver.m"
    File "${SRCDIR}\matlab\gmat_fmincon\EvaluateGMATConstraints.m"
    File "${SRCDIR}\matlab\gmat_fmincon\EvaluateGMATObjective.m"
    File "${SRCDIR}\matlab\gmat_fmincon\GmatFminconOptimizationDriver.m"
    
    ; matlab\gmat_keyword
    SetOutPath "$INSTDIR\matlab\gmat_keyword"
    File "${SRCDIR}\matlab\gmat_keyword\Advise.m"
    File "${SRCDIR}\matlab\gmat_keyword\BuildRunGMAT.m"
    File "${SRCDIR}\matlab\gmat_keyword\CallGMAT.m"
    File "${SRCDIR}\matlab\gmat_keyword\ClearGMAT.m"
    File "${SRCDIR}\matlab\gmat_keyword\CloseGMAT.m"
    File "${SRCDIR}\matlab\gmat_keyword\GetGMATObject.m"
    File "${SRCDIR}\matlab\gmat_keyword\GetGMATVar.m"
    File "${SRCDIR}\matlab\gmat_keyword\OpenGMAT.m"
    File "${SRCDIR}\matlab\gmat_keyword\Poke.m"
    File "${SRCDIR}\matlab\gmat_keyword\Request.m"
    File "${SRCDIR}\matlab\gmat_keyword\RunGMAT.m"
    File "${SRCDIR}\matlab\gmat_keyword\UpdateGMAT.m"
    File "${SRCDIR}\matlab\gmat_keyword\WaitForCallback.m"
    File "${SRCDIR}\matlab\gmat_keyword\WaitForCallbackResults.m"
    File "${SRCDIR}\matlab\gmat_keyword\WaitForGMAT.m"
    
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
    Delete "$INSTDIR\bin\GMAT.exe"
    RMDir "$SMPROGRAMS\GMAT"
    RMDir "$INSTDIR\bin"
    RMDir "$INSTDIR"
    
    ; Start Menu shortcut
    Delete "$SMPROGRAMS\GMAT\R2011a\GMAT.lnk"
    RMDir "$SMPROGRAMS\GMAT\R2011a"
SectionEnd

; Required by MultiUser.nsh
Function .onInit
  !insertmacro MULTIUSER_INIT
FunctionEnd

Function un.onInit
  !insertmacro MULTIUSER_UNINIT
FunctionEnd