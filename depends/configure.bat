:: Author: 		Jfisher
:: Project:		Gmat
:: Title:		configure.bat
:: Purpose:		This script allows developers to quickly and easily 
::			configure the gmat development environment on windows.

:: Turn off output and clear the screen
@echo off

:: Set default variables
set visualstudio_version=10

:: ***********************************
:: Input System
:: ***********************************
:initial
if "%1"=="-vsversion" goto vsversion
if "%1"=="" goto main
goto error

:vsversion
shift
set visualstudio_version=%1
goto lreturn

:lreturn
shift
goto initial

:error
echo %0 usage error
goto end

:main

:: ***********************************
:: Download Library Dependencies
:: ***********************************

:: Check if dependency libraries already exists
set cspice_path=cspice\windows
set wxWidgets_path=wxWidgets\wxMSW-3.0.2

:: Create directories and download cspice if it does not already exist.
IF NOT EXIST %cspice_path% (

	:: Create Directories
	mkdir %cspice_path%
	
	:: Change to cspice directory
	cd %cspice_path%
	
	:: Download and extract Spice (32 and 64), finally remove archive
	IF %processor_architecture% == x86 (
		..\..\bin\winscp\WinSCP.com -script=..\..\bin\cspice\cspice32-ftp.txt
		..\..\bin\7za\7za.exe x cspice.zip
		ren cspice cspice32
		DEL cspice.zip
	) ELSE (
		..\..\bin\winscp\WinSCP.com -script=..\..\bin\cspice\cspice64-ftp.txt
		..\..\bin\7za\7za.exe x cspice.zip
		ren cspice cspice64
		DEL cspice.zip
	)
        
	:: Change back to depends directory
        cd ..\..
)

:: Create directories and download wxwidgets if it does not already exist.
IF NOT EXIST %wxWidgets_path% (

	:: Create Directories
	mkdir %wxWidgets_path%
	
	:: Change to wxwidgets directory
	cd %wxWidgets_path%
	
	:: Download wxWidgets
        ..\..\bin\winscp\WinSCP.com -script=..\..\bin\wx\wx-ftp.txt


	:: Extract wxWidgets
	..\..\bin\7za\7za.exe x wxMSW-3.0.2_vc%visualstudio_version%0_x64_Dev.7z
	..\..\bin\7za\7za.exe x wxMSW-3.0.2_vc%visualstudio_version%0_x64_ReleaseDLL.7z
	..\..\bin\7za\7za.exe x wxWidgets-3.0.2_headers.7z
	DEL wxWidgets-3.0.2_headers.7z 
	DEL wxMSW-3.0.2_vc%visualstudio_version%0_x64_ReleaseDLL.7z
	DEL wxMSW-3.0.2_vc%visualstudio_version%0_x64_Dev.7z

	:: Change dll folder name
	cd lib
	REN vc%visualstudio_version%0_x64_dll vc_x64_dll

        cd ..\..\..
)

echo Dependency Configuration Complete!

:: ***********************************
:: End of script
:: ***********************************
:end
