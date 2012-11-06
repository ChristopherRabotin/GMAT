:: Author: 		Jfisher
:: Project:		Gmat
:: Title:		configure.bat
:: Purpose:		This script allows developers to quickly and easily 
::				configure the gmat development environment on windows.

:: Turn off output and clear the screen
@ECHO OFF
cls

:: Set default variables
set gmat_path=C:\dev\aisolutions\gmat-buildbranch\
set wget="C:\Program Files (x86)\GnuWin32\bin\wget.exe"
set svn="C:\Program Files\TortoiseSVN\bin\svn.exe"
set sevenz="C:\Program Files\7-Zip\7z.exe"
set visualstudio_version=10.0
set vs_path="C:\Program Files\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"
set sdk_path="C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.Cmd"
set use_latest=false

:: ***********************************
:: Input System
:: ***********************************
:initial
if "%1"=="-p" goto path
if "%1"=="-wget" goto wget
if "%1"=="-svn" goto svn
if "%1"=="-sevenz" goto sevenz
if "%1"=="-vsversion" goto vsversion
if "%1"=="-vspath" goto vspath
if "%1"=="-latest" goto latest
if "%1"=="" goto main
goto error

:path
shift
set gmat_path=%1
goto lreturn

:wget
shift
set wget=%1
goto lreturn

:svn
shift
set svn=%1
goto lreturn

:sevenz
shift
set sevenz=%1
goto lreturn

:vsversion
shift
set visualstudio_version=%1
goto lreturn

:vspath
shift
set vs_path=%1
goto lreturn

:latest
shift
set use_latest=%1

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

:: Set wget path 1 
set wget_p1="C:\Program Files (x86)\GnuWin32\bin\wget.exe"
set wget_p2="C:\Program Files\GnuWin32\bin\wget.exe"

:: Set subversion based on architecture
set svn_p1="C:\Program Files (x86)\Subversion\bin\svn.exe"
set svn_p2="C:\Program Files\Subversion\bin\svn.exe"

:: Set sevenzip based on architecture
set sevenz_p1="C:\Program Files (x86)\7-Zip\7z.exe"
set sevenz_p2="C:\Program Files\7-Zip\7z.exe"

:: Set ms visual studio location based on architecture
IF %processor_architecture% == x86 (
	set vs_p1="C:\Program Files\Microsoft Visual Studio %visualstudio_version%\VC\vcvarsall.bat"
) ELSE (
	set vs_p1="C:\Program Files (x86)\Microsoft Visual Studio %visualstudio_version%\VC\vcvarsall.bat"
)

::Validate gmat path
IF NOT EXIST %gmat_path% (
	ECHO ...............................................
	ECHO Arguments are invalid. Please consult the
	ECHO documentation for proper input argument syntax.
	ECHO ...............................................
	ECHO.
	GOTO end
)


:: Validate/Configure wget path
IF NOT EXIST %wget% (
	IF NOT EXIST %wget_p1% (
		IF NOT EXIST %wget_p2% (
			ECHO ...............................................
			ECHO Dependency software is not installed. Please
			ECHO consult the documentation for the required
			ECHO software prerequisites for this script.
			ECHO ...............................................
			ECHO.
		) ELSE (
			set wget=%wget_p2%
		)
	) ELSE (
		set wget=%wget_p1%
	)
)

:: Validate/Configure svn path
IF NOT EXIST %svn% (
	IF NOT EXIST %svn_p1% (
		IF NOT EXIST %svn_p2% (
			ECHO ...............................................
			ECHO Dependency software is not installed. Please
			ECHO consult the documentation for the required
			ECHO software prerequisites for this script.
			ECHO ...............................................
			ECHO.
		) ELSE (
			set svn=%svn_p2%
		)
	) ELSE (
		set svn=%svn_p1%
	)
)

:: Validate/Configure sevenz path
IF NOT EXIST %sevenz% (
	IF NOT EXIST %sevenz_p1% (
		IF NOT EXIST %sevenz_p2% (
			ECHO ...............................................
			ECHO Dependency software is not installed. Please
			ECHO consult the documentation for the required
			ECHO software prerequisites for this script.
			ECHO ...............................................
			ECHO.
		) ELSE (
			set sevenz=%sevenz_p2%
		)
	) ELSE (
		set sevenz=%sevenz_p1%
	)
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
set bin_path=%gmat_path%\depends\bin
set f2c_path=%gmat_path%\depends\f2c
set cspice_path=%gmat_path%\depends\cspice
set wxWidgets_path=%gmat_path%\depends\wxWidgets
set sofa_path=%gmat_path%\depends\sofa
set tsplot_path=%gmat_path%\depends\tsPlot
set pcre_path=%gmat_path%\depends\pcre

:: Extract f2c Binaries
IF NOT EXIST %bin_path%\f2c\f2c32 (

	cd %bin_path%\f2c
	%sevenz% x f2c32.zip -o%bin_path%\f2c\f2c32
	%sevenz% x f2c64.zip -o%bin_path%\f2c\f2c64
)

:: Create directories and download f2c if it does not already exist.
IF NOT EXIST %f2c_path% (

	:: Create Directories
	mkdir %f2c_path%
	
	:: Change to f2c directory
	cd %f2c_path%
	
	:: Use wget to download software
	%wget% -nH --cut-dirs=1 -r ftp://netlib.org/f2c/
)

:: Create directories and download cspice if it does not already exist.
IF NOT EXIST %cspice_path% (

	:: Create Directories
	mkdir %cspice_path%
	
	:: Change to cspice directory
	cd %cspice_path%
	
	:: - Download and extract Spice (32 and 64), finally remove archive
	%wget% ftp://naif.jpl.nasa.gov/pub/naif/toolkit//C/PC_Windows_VisualC_32bit/packages/cspice.zip
	%sevenz% x cspice.zip
	ren cspice cspice32
	DEL cspice.zip
	
	%wget% ftp://naif.jpl.nasa.gov/pub/naif/toolkit//C/PC_Windows_VisualC_64bit/packages/cspice.zip
	%sevenz% x cspice.zip
	ren cspice cspice64
	DEL cspice.zip
)

:: Create directories and download wxwidgets if it does not already exist.
IF NOT EXIST %wxWidgets_path% (
	
	:: Create Directories
	mkdir %wxWidgets_path%
	
	:: Change to wxwidgets directory
	cd %wxWidgets_path%
	
	:: - Checkout wxWidget source.
	%svn% co http://svn.wxwidgets.org/svn/wx/wxWidgets/tags/WX_2_8_12/ wxWidgets-2.8.12
	
	:: Copy modified wxWidget setup.h file to downloaded source path (Preconfigured to use
	:: ODBC and GLCanvas
	copy "%bin_path%\wx\setup.h" "%wxWidgets_path%\wxWidgets-2.8.12\include\wx\msw\setup.h" /Y
	copy "%bin_path%\wx\setup.h" "%wxWidgets_path%\wxWidgets-2.8.12\include\wx\msw\setup0.h" /Y
	
	IF %use_latest% == true (
		:: Change to wxwidgets directory
		cd %wxWidgets_path%
	
		:: - Checkout latest wxWidget source.
		%svn%  co http://svn.wxwidgets.org/svn/wx/wxWidgets/trunk/ wxWidgets-latest
		
		:: Copy modified wxWidget setup.h file to downloaded source path (Preconfigured to use
		:: ODBC and GLCanvas
		copy "%bin_path%\wx\setup.h" "%wxWidgets_path%\wxWidgets-latest\include\wx\msw\setup0.h" /Y
		copy "%bin_path%\wx\setup.h" "%wxWidgets_path%\wxWidgets-latest\include\wx\msw\setup0.h" /Y
		
	)
)

:: Create directories and download sofa if it does not already exist.
IF NOT EXIST %sofa_path% (
	
	::Change to depends directory
	cd %gmat_path%\depends\

	:: - Download and extract Sofa Source, finally remove archive
	%wget% http://www.iausofa.org/2012_0301_C/sofa_c_a.zip
	%sevenz% x sofa_c_a.zip
	DEL sofa_c_a.zip
)

:: Create directories and download tsplot if it does not already exist.
IF NOT EXIST %tsplot_path% (

	:: Create Directories
	mkdir %tsplot_path%
	
	:: Change to tsplot directory
	cd %tsplot_path%
	
	:: - Checkout wxWidget source.
	%svn% co svn://svn.code.sf.net/p/tsplot/code/trunk tsplot-latest
)

:: Create directories and download pcre if it does not already exist.
IF NOT EXIST %pcre_path% (

	:: Create Directories
	mkdir %pcre_path%
	
	:: Change to pcre directory
	cd %pcre_path%
	
	:: - Checkout pcre source.
	%svn% co svn://vcs.exim.org/pcre/code/tags/pcre-8.31 pcre-8.31
	
	IF %use_latest% == true (
		
		:: Change to pcre directory
		cd %pcre_path%
	
		:: - Checkout latest pcre source.
		%svn%  co svn://vcs.exim.org/pcre/code/trunk pcre-latest
	)
)


:: ***********************************
:: Build Library Dependencies
:: ***********************************

:: Set build path based on version
IF %use_latest% == true (
	set wx_build_path=%gmat_path%\depends\wxWidgets\latest\build\msw
) ELSE (
	set wx_build_path=%gmat_path%\depends\wxWidgets\wxWidgets-2.8.12\build\msw
)

:: Check if dependencies have already been built
set depend_x86_path="%gmat_path%\depends\wxWidgets\wxWidgets-2.8.12\lib\vc_dll\wxmsw28_core_vc_custom.dll"
set depend_amd64_path="%gmat_path%\depends\wxWidgets\wxWidgets-2.8.12\lib\vc_amd64_dll\wxmsw28_core_vc_custom.dll"

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
cls
echo Dependency Configuration Complete!
pause
:end

