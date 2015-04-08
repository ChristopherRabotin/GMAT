:: Author: 		Jfisher (Revised by Ravi Mathur)
:: Project:		Gmat
:: Title:		configure.bat
:: Purpose:		This script allows developers to quickly and easily 
::			configure the gmat development environment on windows.

:: Turn off output and clear the screen
@echo off
cls

:: Set default variables
set vs_version=12
set use_64bit=1

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
if "%user_vs"=="2008" ( set vs_version=9 )
if "%user_vs"=="2010" ( set vs_version=10 )
if "%user_vs"=="2012" ( set vs_version=11 )
if "%user_vs"=="2013" ( set vs_version=12 )
goto main

:error
echo %0 usage error
goto end

:main

IF %use_64bit% EQU 1 (
	echo ********** Setting up 64-bit dependencies **********
) ELSE (
	echo ********** Setting up 32-bit dependencies **********
)

:: ***********************************
:: Download Library Dependencies
:: ***********************************

:: Check if dependency libraries already exists
:: Note that cspice_type is used in cspice-ftp.txt
set cspice_path=cspice\windows
IF %use_64bit% EQU 1 (
	set cspice_dir=cspice64
	set cspice_type=64bit
) ELSE (
	set cspice_dir=cspice32
	set cspice_type=32bit
)

:: Create directories and download cspice if it does not already exist.
IF NOT EXIST %cspice_path%\%cspice_dir% (

	:: Create Directories
	mkdir %cspice_path%
	
	:: Change to cspice directory
	cd %cspice_path%
	
	:: Download and extract Spice, finally remove archive
	..\..\bin\winscp\WinSCP.com -script=..\..\bin\cspice\cspice-ftp.txt
	..\..\bin\7za\7za.exe x cspice.zip
	REN cspice %cspice_dir%
	DEL cspice.zip
        
	:: Change back to depends directory
        cd ..\..
) ELSE (
	echo CSpice already exists
)

echo ********** Setting up wxWidgets for VisualStudio version %vs_version%.0 **********

set wxWidgets_path=wxWidgets\wxMSW-3.0.2
IF %use_64bit% EQU 1 (
	set wxwidgets_type=_x64_
) ELSE (
	set wxwidgets_type=_
)

:: Create directories and download wxwidgets if it does not already exist.
:: Note that vs_version and wxwidgets_type are used in wx-ftp.txt
IF NOT EXIST %wxWidgets_path%\lib\vc%wxwidgets_type%dll (

	:: Create Directories
	mkdir %wxWidgets_path%
	
	:: Change to wxwidgets directory
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

        cd ..\..\..
) ELSE (
	echo wxWidgets already exists
)

echo Dependency Configuration Complete!

:: ***********************************
:: End of script
:: ***********************************
:end
