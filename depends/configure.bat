:: Author: 		Jfisher
:: Project:		Gmat
:: Title:		configure.bat
:: Purpose:		This script allows developers to quickly and easily 
::			configure the gmat development environment on windows.
:: Updates: Feb-Apr 2015: Ravi Mathur: Heavy updates for new CMake
:: Use:
::   This script can be used via standard double-click or in the Windows
::   command prompt. Command prompt usage is:
::    depends.bat [-vsversion (9|10|11|12)] [-x86 | -x64]
::    -vsversion (9|10|11|[12]) : Visual Studio version (default 12)
::    -x86 | -x64 : Build 32 (x86) or 64 (x64) bit CSPICE (default x64)
::   Note that VisualStudio versions are:
::    12 = VS2013
::    11 = VS2012
::    10 = VS2010
::     9 = VS2008

:: Turn off output and clear the screen
@echo off
cls

:: Set default variables
set vs_version=12
set use_64bit=1
set depends_dir=%cd%

:: ***********************************
:: Input System
:: ***********************************
:initial
if "%1"=="-vsversion" goto vsversion
if "%1"=="-x64" goto vs64
if "%1"=="-x86" goto vs32 
if "%1"=="" goto gettype
goto error

:vsversion
shift
set vs_version=%1
goto lreturn

:vs64
set use_64bit=1
goto lreturn

:vs32
set use_64bit=0
goto lreturn

:lreturn
shift
if "%1"=="" goto main
goto initial

:gettype
set /p user_bit="Use 32/64 bit? [32/(64)]: " || set user_bit="64"
if "%user_bit%"=="32" (
	set use_64bit=0
) else (
	set use_64bit=1
)

set /p user_vs="VisualStudio Version? [2008/2010/2012/[2013]]: " || set user_vs="2013"
if "%user_vs%"=="2008" (set vs_version=9)
if "%user_vs%"=="2010" (set vs_version=10)
if "%user_vs%"=="2012" (set vs_version=11)
if "%user_vs%"=="2013" (set vs_version=12)
goto main

:error
echo %0 usage error
goto end

:main

IF %use_64bit% EQU 1 (
	echo ********** Setting up GMAT 64-bit dependencies for VisualStudio %vs_version%.0 **********
) ELSE (
	echo ********** Setting up GMAT 32-bit dependencies for VisualStudio %vs_version%.0 **********
)

set logs_dir=%depends_dir%\logs
IF NOT EXIST %logs_dir% (
	mkdir %logs_dir%
)

:: Check if dependency libraries already exists
:: Note that cspice_type is used in cspice-ftp.txt
set cspice_path=cspice\windows
IF %use_64bit% EQU 1 (
	set cspice_dir=cspice64
	set cspice_type=64bit
	set vs_arch=x86_amd64
) ELSE (
	set cspice_dir=cspice32
	set cspice_type=32bit
	set vs_arch=x86
)

:: Add Visual Studio tools to command line path
set vs_envvar=vs%vs_version%0comntools
setlocal enabledelayedexpansion
for /F %%a in ("%vs_envvar%") do set vs_path=!%%a!
IF "%vs_path%" == "" (
	echo ***Visual Studio %vs_version%.0 NOT FOUND! Environment variable %vs_envvar% missing. ***
	echo Please enter full path to Microsoft Visual Studio %vs_version%.0 folder
	set /p vs_base_path="Path: "
	set vs_path=!vs_base_path!\Microsoft Visual Studio %vs_version%.0\Common7\Tools\
)
endlocal & set vs_path=%vs_path%
call "%vs_path%\..\..\VC\vcvarsall.bat" %vs_arch%

echo.
echo ********** Configuring CSPICE **********

:: Create directories and download CSPICE if it does not already exist
IF NOT EXIST %cspice_path%\%cspice_dir% (
	:: Create Directories
	mkdir %cspice_path%
	
	:: Change to cspice directory
	cd %cspice_path%
	
	:: Download and extract CSPICE
	echo -- Downloading CSPICE
	..\..\bin\winscp\WinSCP.com -script=..\..\bin\cspice\cspice-ftp.txt
	..\..\bin\7za\7za.exe x cspice.zip > nul
	REN cspice %cspice_dir%
	DEL cspice.zip

	:: Change back to depends directory
	cd "%depends_dir%"
)

:: Compile CSPICE
IF NOT EXIST %cspice_path%\%cspice_dir%\lib\cspiced.lib (
	:: Compile debug CSPICE. See GMT-5044
	echo -- Compiling debug CSPICE. This could take a while...
	cd %cspice_dir%\src\cspice
	cl /c /DEBUG /Z7 /MP -D_COMPLEX_DEFINED -DMSDOS -DOMIT_BLANK_CC -DNON_ANSI_STDIO -DUIOLEN_int *.c > %logs_dir%\cspice_build_debug.log 2>&1
	link -lib /out:..\..\lib\cspiced.lib *.obj >> %logs_dir%\cspice_build_debug.log 2>&1
	del *.obj

	:: Compile release CSPICE. See GMT-5044
	echo -- Compiling release CSPICE. This could take a while...
	cl /c /O2 /MP -D_COMPLEX_DEFINED -DMSDOS -DOMIT_BLANK_CC -DNON_ANSI_STDIO -DUIOLEN_int *.c > %logs_dir%\cspice_build_release.log 2>&1
	link -lib /out:..\..\lib\cspice.lib *.obj >> %logs_dir%\cspice_build_release.log 2>&1
	del *.obj
        
	:: Change back to depends directory
	cd "%depends_dir%"
) ELSE (
	echo -- CSPICE already configured
)

echo.
echo ********** Configuring wxWidgets **********

set wxWidgets_path=wxWidgets\wxMSW-3.0.2
IF %use_64bit% EQU 1 (
	set wxwidgets_type=_x64_
) ELSE (
	set wxwidgets_type=_
)

:: Create directories and download wxWidgets if it does not already exist.
:: Note that vs_version and wxwidgets_type are used in wx-ftp.txt
IF NOT EXIST %wxWidgets_path%\lib\vc%wxwidgets_type%dll (

	:: Create Directories
	mkdir %wxWidgets_path%
	
	:: Change to wxWidgets directory
	cd %wxWidgets_path%
	
	:: Download wxWidgets
        ..\..\bin\winscp\WinSCP.com -script=..\..\bin\wx\wx-ftp.txt

	:: Extract wxWidgets
	IF NOT EXIST include (
		..\..\bin\7za\7za.exe x wxWidgets-3.0.2_headers.7z
	)
	..\..\bin\7za\7za.exe x wxMSW-3.0.2_vc%vs_version%0%wxwidgets_type%Dev.7z
	..\..\bin\7za\7za.exe x wxMSW-3.0.2_vc%vs_version%0%wxwidgets_type%ReleaseDLL.7z

	DEL wxWidgets-3.0.2_headers.7z 
	DEL wxMSW-3.0.2_vc%vs_version%0%wxwidgets_type%ReleaseDLL.7z
	DEL wxMSW-3.0.2_vc%vs_version%0%wxwidgets_type%Dev.7z

	:: Change dll folder name
	cd lib
	REN vc%vs_version%0%wxwidgets_type%dll vc%wxwidgets_type%dll

        cd "%depends_dir%"
) ELSE (
	echo -- wxWidgets already configured
)

echo.
echo Dependency Configuration Complete!
echo.

:: ***********************************
:: End of script
:: ***********************************
:end
