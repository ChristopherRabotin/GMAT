REM Author: 		Jfisher
REM Project:		Gmat
REM Title:		configure.bat
REM Purpose:		This script allows developers to quickly and easily 
REM 			configure the GMAT software dependencies on Windows.
REM Updates: Feb-Apr 2015: Ravi Mathur: Heavy updates for new CMake
REM              Jul 2016: Ravi Mathur: Updates for Xerces and Python
REM Use:
REM   This script can be used via standard double-click or in the Windows
REM   command prompt. Command prompt usage is:
REM    depends.bat [-vsversion (9|10|11|12)] [-x86 | -x64]
REM    -vsversion (9|10|11|[12]|14) : Visual Studio version (default 12)
REM    -x86 | -x64 : Build 32 (x86) or 64 (x64) bit dependencies (default x64)
REM   Note that VisualStudio versions are:
REM    14 = VS2015 (not yet supported by wxWidgets)
REM    12 = VS2013
REM    11 = VS2012
REM    10 = VS2010
REM     9 = VS2008

REM Turn off output and clear the screen
@echo off
cls

REM Set default variables
set vs_version=12
set use_64bit=1
set depends_dir=%cd%
set run_curl=%depends_dir%\bin\curl\curl.exe
set run_7za=%depends_dir%\bin\7za\7za.exe

REM ***********************************
REM Input System
REM ***********************************
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
if "%user_vs%"=="2015" (set vs_version=14)
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

REM Add Visual Studio tools to command line path
IF %use_64bit% EQU 1 (
	set vs_arch=x86_amd64
) ELSE (
	set vs_arch=x86
)
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

set cspice_path=cspice\windows
IF %use_64bit% EQU 1 (
	set cspice_dir=cspice64
	set cspice_type=64bit
) ELSE (
	set cspice_dir=cspice32
	set cspice_type=32bit
)

REM Create directories and download CSPICE if it does not already exist
IF NOT EXIST %cspice_path%\%cspice_dir% (
	REM Create Directories
	mkdir %cspice_path%
	
	REM Change to cspice directory
	cd %cspice_path%
	
	REM Download and extract CSPICE
	echo -- Downloading CSPICE
	"%run_curl%" http://naif.jpl.nasa.gov/pub/naif/toolkit/C/PC_Windows_VisualC_%cspice_type%/packages/cspice.zip > cspice.zip
	
	"%run_7za%" x cspice.zip > nul
	REN cspice %cspice_dir%
	DEL cspice.zip

	REM Change back to depends directory
	cd "%depends_dir%"
)

REM Compile CSPICE
IF NOT EXIST %cspice_path%\%cspice_dir%\lib\cspiced.lib (
	REM Compile debug CSPICE. See GMT-5044
	echo.
	echo -- Compiling debug CSPICE. This could take a while...
	cd %cspice_path%\%cspice_dir%\src\cspice
	cl /c /DEBUG /Z7 /MP -D_COMPLEX_DEFINED -DMSDOS -DOMIT_BLANK_CC -DNON_ANSI_STDIO -DUIOLEN_int *.c > %logs_dir%\cspice_build_debug.log 2>&1
	link -lib /out:..\..\lib\cspiced.lib *.obj >> %logs_dir%\cspice_build_debug.log 2>&1
	del *.obj

	REM Compile release CSPICE. See GMT-5044
	echo.
	echo -- Compiling release CSPICE. This could take a while...
	cl /c /O2 /MP -D_COMPLEX_DEFINED -DMSDOS -DOMIT_BLANK_CC -DNON_ANSI_STDIO -DUIOLEN_int *.c > %logs_dir%\cspice_build_release.log 2>&1
	link -lib /out:..\..\lib\cspice.lib *.obj >> %logs_dir%\cspice_build_release.log 2>&1
	del *.obj
        
	REM Change back to depends directory
	cd "%depends_dir%"
) ELSE (
	echo -- CSPICE already configured
)

set wxver=3.0.2
echo.
echo ********** Configuring wxWidgets %wxver% **********

IF %use_64bit% EQU 1 (
	set wxtype=_x64_
) ELSE (
	set wxtype=_
)
set wxhdr=wxWidgets-%wxver%_headers.7z
set wxdev=wxMSW-%wxver%_vc%vs_version%0%wxtype%%Dev.7z
set wxrel=wxMSW-%wxver%_vc%vs_version%0%wxtype%%ReleaseDLL.7z
set wxWidgets_path=wxWidgets\wxMSW-%wxver%

REM Create directories and download wxWidgets if it does not already exist.
IF NOT EXIST %wxWidgets_path%\lib\vc%wxtype%dll (
	REM Create Directories
	mkdir %wxWidgets_path%
	
	REM Change to wxWidgets directory
	cd %wxWidgets_path%
	
	REM Download wxWidgets headers and binaries
	echo -- Downloading wxWidgets headers
	%run_curl% -Lk https://github.com/wxWidgets/wxWidgets/releases/download/v%wxver%/%wxhdr% > %wxhdr%

	echo.
	echo -- Downloading wxWidgets debug libraries
	%run_curl% -Lk https://github.com/wxWidgets/wxWidgets/releases/download/v%wxver%/%wxdev% > %wxdev%

	echo.
	echo -- Downloading wxWidgets release libraries
	%run_curl% -Lk https://github.com/wxWidgets/wxWidgets/releases/download/v%wxver%/%wxrel% > %wxrel%

	REM Extract wxWidgets
	IF NOT EXIST include (
		%run_7za% x %wxhdr% > nul
	)
	%run_7za% x %wxdev% > nul
	%run_7za% x %wxrel% > nul

	DEL %wxhdr%
	DEL %wxdev%
	DEL %wxrel%

	REM Change dll folder name
	cd lib
	REN vc%vs_version%0%wxtype%dll vc%wxtype%dll

	REM Change back to depends directory
        cd "%depends_dir%"
) ELSE (
	echo -- wxWidgets already configured
)

set xercesver=3.1.4
echo.
echo ********** Configuring Xerces-C++ %xercesver% **********

set xerces_path=xerces
IF %use_64bit% EQU 1 (
	set xerces_outdir=%xerces_path%\Build\Win64
	set xerces_arch=x64
) ELSE (
	set xerces_outdir=%xerces_path%\Build\Win32
	set xerces_arch=Win32
)

REM Download Xerces if it doesn't exist
IF NOT EXIST %xerces_path% (
	REM Download and extract Xerces
	echo -- Downloading Xerces
	"%run_curl%" http://archive.apache.org/dist/xerces/c/3/sources/xerces-c-%xercesver%.tar.gz > xerces.tar.gz
	"%run_7za%" x xerces.tar.gz > nul
	"%run_7za%" x xerces.tar > nul
	DEL xerces.tar.gz
	DEL xerces.tar
	REN xerces-c-%xercesver% %xerces_path%

	REM Change back to depends directory
	cd "%depends_dir%"
)

REM Compile Xerces
IF NOT EXIST %xerces_outdir% (
	REM Compile debug Xerces
	REM XercesLibOverride is needed because the Xerces Runtime Library cannot be directly changed with an msbuild flag
	echo.
	echo -- Compiling debug Xerces. This could take a while...
	cd %xerces_path%\projects\Win32\VC%vs_version%\xerces-all\XercesLib
	msbuild /m ^
		/property:Configuration="Static Debug";Platform=%xerces_arch% ^
		/property:TargetName=xerces-c_3D ^
		/property:ForceImportBeforeCppTargets="%depends_dir%\bin\xerces\XercesLibOverride.prop" ^
		XercesLib.vcxproj > %logs_dir%\xerces_build_debug.log 2>&1

	REM Compile release Xerces
	echo.
	echo -- Compiling release Xerces. This could take a while...
	msbuild /m ^
		/property:Configuration="Static Release";Platform=%xerces_arch% ^
		/property:TargetName=xerces-c_3 ^
		/property:ForceImportBeforeCppTargets="%depends_dir%\bin\xerces\XercesLibOverride.prop" ^
		XercesLib.vcxproj > %logs_dir%\xerces_build_release.log 2>&1

	REM Change back to depends directory
	cd "%depends_dir%"
) ELSE (
	echo -- Xerces already configured
)

echo.
echo Dependency Configuration Complete!
echo.

REM ***********************************
REM End of script
REM ***********************************
:end
pause
