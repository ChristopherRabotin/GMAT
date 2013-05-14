:: Author: 		Jfisher
:: Project:		Gmat
:: Title:		configure.bat
:: Purpose:		This script allows developers to quickly and easily 
::				configure the gmat development environment on windows.

:: Turn off output and clear the screen
@echo off

:: Set default variables
set gmat_path=..
set visualstudio_version=10.0
set vs_path="C:\Program Files\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"
set sdk_path="C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.Cmd"

:: ***********************************
:: Input System
:: ***********************************
:initial
if "%1"=="-vsversion" goto vsversion
if "%1"=="-vspath" goto vspath
if "%1"=="" goto main
goto error

:vsversion
shift
set visualstudio_version=%1
goto lreturn

:vspath
shift
set vs_path=%1
goto lreturn

:lreturn
shift
goto initial

:error
echo %0 usage error
goto end

:main

:: ***********************************
:: Configure Paths
:: ***********************************

:: Set ms visual studio location based on architecture
IF %processor_architecture% == x86 (
	set vs_p1="C:\Program Files\Microsoft Visual Studio %visualstudio_version%\VC\vcvarsall.bat"
) ELSE (
	set vs_p1="C:\Program Files (x86)\Microsoft Visual Studio %visualstudio_version%\VC\vcvarsall.bat"
)

:: Validate/Configure visual studio path
set p0=%vs_path%
set p1=%vs_p1%
IF NOT EXIST %p0% (
	IF NOT EXIST %p1% (
		IF NOT EXIST %sdk_path% (
			ECHO ...............................................
			ECHO Dependency software is not installed. Please
			ECHO consult the documentation for the required
			ECHO software prerequisites for this script.
			ECHO ...............................................
			ECHO.
		) ELSE (
			set vs_path=%p1%
		)
	) ELSE (
		set vs_path=%p1%
	)
)


:: ***********************************
:: Download Library Dependencies
:: ***********************************

:: Check if dependency libraries already exists
set cspice_path=cspice
set wxWidgets_path=wxWidgets

:: Create directories and download cspice if it does not already exist.
IF NOT EXIST %cspice_path% (

	:: Create Directories
	mkdir %cspice_path%
	
	:: Change to cspice directory
	cd %cspice_path%
	
	:: - Download and extract Spice (32 and 64), finally remove archive
	ftp -s:..\bin\cspice\cspice32-ftp.txt
	..\bin\unzip\unzip cspice.zip
	ren cspice cspice32
	DEL cspice.zip
	
	ftp -s:..\bin\cspice\cspice64-ftp.txt
	..\bin\unzip\unzip cspice.zip
	ren cspice cspice64
	DEL cspice.zip
        
        cd ..
)

:: Create directories and download wxwidgets if it does not already exist.
IF NOT EXIST %wxWidgets_path% (
	
	:: Create Directories
	mkdir %wxWidgets_path%
	
	:: Change to wxwidgets directory
	cd %wxWidgets_path%
	
	:: - Download wxWidgets
        ftp -s:..\bin\wx\wx-ftp.txt
	
	:: - Unzip wxWidgets
        ..\bin\unzip\unzip wxMSW-2.8.12.zip
	
	:: Copy modified wxWidget setup.h file to downloaded source path (Preconfigured to use
	:: ODBC and GLCanvas
	copy "..\bin\wx\setup.h" "wxMSW-2.8.12\include\wx\msw\setup.h" /Y
	copy "..\bin\wx\setup.h" "wxMSW-2.8.12\include\wx\msw\setup0.h" /Y

        cd ..
)

:: ***********************************
:: Build Library Dependencies
:: ***********************************

:: Set build path based on version
set wx_build_path=wxWidgets\wxMSW-2.8.12\build\msw

:: Check if dependencies have already been built
set depend_x86_path="wxWidgets\wxMSW-2.8.12\lib\vc_dll\wxmsw28_core_vc_custom.dll"
set depend_amd64_path="wxWidgets\wxMSW-2.8.12\lib\vc_amd64_dll\wxmsw28_core_vc_custom.dll"

cd %wx_build_path%

IF NOT EXIST %depend_x86_path% (

	:: Launch MS Build environmental variables for build process
	IF EXIST %sdk_path% (
		call %sdk_path% /Release /x86 /xp
	) ELSE (
		call %vs_path% x86
	)

	:: - Compile 32bit wxWidget source (Clean, static, dynamic)
	nmake -f makefile.vc clean SHARED=0 USE_OPENGL=1 USE_ODBC=1 BUILD=release
	nmake -f makefile.vc all SHARED=0 USE_OPENGL=1 USE_ODBC=1 BUILD=release
	nmake -f makefile.vc clean SHARED=1 USE_OPENGL=1 USE_ODBC=1 BUILD=release
	nmake -f makefile.vc all SHARED=1 USE_OPENGL=1 USE_ODBC=1 BUILD=release
	
	:: - Change to contrib directory
	cd ../../contrib/build/net/

	:: - Compile 32 bit wxWidget contrib/net source (Clean, static, dynamic)
	nmake -f makefile.vc clean SHARED=0 BUILD=release
	nmake -f makefile.vc all SHARED=0 BUILD=release
	nmake -f makefile.vc clean SHARED=1 BUILD=release
	nmake -f makefile.vc all SHARED=1 BUILD=release
	
	:: - Compile 32bit wxWidget contrib/stc source (Clean, static, dynamic)
	cd ../stc
	nmake -f makefile.vc clean SHARED=0 BUILD=release
	nmake -f makefile.vc all SHARED=0 BUILD=release
	nmake -f makefile.vc clean SHARED=1 BUILD=release
	nmake -f makefile.vc all SHARED=1 BUILD=release	

)

IF NOT EXIST %depend_amd64_path% (

	cd %wx_build_path%

	:: Launch MS Build environmental variables for build process
	IF EXIST %sdk_path% (
		call %sdk_path% /Release /x64 /xp
	) ELSE (
		IF %processor_architecture% == x86 (
			call %vs_path% x86_amd64
		) ELSE (
			call %vs_path% amd64
		)
	)

	:: - Compile 64bit wxWidget source (Clean, static, dynamic)
	nmake -f makefile.vc clean SHARED=0 USE_OPENGL=1 USE_ODBC=1 BUILD=release TARGET_CPU=AMD64
	nmake -f makefile.vc all SHARED=0 USE_OPENGL=1 USE_ODBC=1 BUILD=release TARGET_CPU=AMD64
	nmake -f makefile.vc clean SHARED=1 USE_OPENGL=1 USE_ODBC=1 BUILD=release TARGET_CPU=AMD64
	nmake -f makefile.vc all SHARED=1 USE_OPENGL=1 USE_ODBC=1 BUILD=release TARGET_CPU=AMD64
	
	:: - Change to contrib directory
	cd ../../contrib/build/net/
	
	:: - Compile 64bit wxWidget contrib/net source (Clean, static, dynamic)
	nmake -f makefile.vc clean SHARED=0 BUILD=release TARGET_CPU=AMD64
	nmake -f makefile.vc all SHARED=0 BUILD=release TARGET_CPU=AMD64
	nmake -f makefile.vc clean SHARED=1 BUILD=release TARGET_CPU=AMD64
	nmake -f makefile.vc all SHARED=1 BUILD=release TARGET_CPU=AMD64
	
	:: - Compile 64bit wxWidget contrib/stc source (Clean, static, dynamic)
	cd ../stc
	nmake -f makefile.vc clean SHARED=0 BUILD=release TARGET_CPU=AMD64
	nmake -f makefile.vc all SHARED=0 BUILD=release TARGET_CPU=AMD64
	nmake -f makefile.vc clean SHARED=1 BUILD=release TARGET_CPU=AMD64
	nmake -f makefile.vc all SHARED=1 BUILD=release TARGET_CPU=AMD64
	
)

:: Move back to default path
cd %gmat_path%

:: ***********************************
:: End of script
:: ***********************************
echo Dependency Configuration Complete!
:end

