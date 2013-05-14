:: Author: 		Jfisher
:: Project:		Gmat
:: Title:		build.bat
:: Purpose:		Windows Script to configure and easily build gmat for end users
:: Usage: 		-arch [x86 | x64] -target [debug | release] -version [10.0 | 11.0) -cmake [/path/to/cmake/]

:: Turn off output and clear the screen
@ECHO OFF
cls

:: Intro Text
ECHO ........................................................
ECHO Starting GMAT Build
ECHO ........................................................
ECHO.

:: Set default variables
set arch=x86
set target=release
set version=10.0
set wx_output_x86_folder=..\application\bin\
set wx_lib_x86_folder=..\depends\wxWidgets\wxMSW-2.8.12\lib\vc_dll\
set wx_output_amd64_folder=..\application\bin\
set wx_lib_amd64_folder=..\depends\wxWidgets\wxMSW-2.8.12\lib\vc_amd64_dll\

:: ***********************************
:: Input Args
:: ***********************************
:initial
if "%1"=="-arch" goto arch
if "%1"=="-target" goto target
if "%1"=="-version" goto version
if "%1"=="" goto main
goto error

:arch
shift
set arch=%1
goto lreturn

:target
shift
set target=%1
goto lreturn

:lreturn
shift
goto initial

:version
shift
set version=%1
goto lreturn

:error
echo %0 Arguments are Invalid. See usage documentation.
goto end

:main

:: Copy wxWidget libs to /application/bin depending on user -arch type
IF %arch% == x86 (
	:: Clean output path
	DEL "%wx_output_amd64_folder%\*.exe" /Q
	DEL "%wx_output_amd64_folder%\*.dll" /Q
	DEL "%wx_output_amd64_folder%\*.exp" /Q
	DEL "%wx_output_amd64_folder%\*.lib" /Q

	:: Copy wxWidget dll's to gmat application/bin/arch directory
	copy "%wx_lib_x86_folder%\*.dll" "%wx_output_x86_folder%"
) ELSE (
	:: Clean output path
	DEL "%wx_output_amd64_folder%\*.exe" /Q
	DEL "%wx_output_amd64_folder%\*.dll" /Q
	DEL "%wx_output_amd64_folder%\*.exp" /Q
	DEL "%wx_output_amd64_folder%\*.lib" /Q

	:: Copy wxWidget dll's to gmat application/bin/arch directory
	copy "%wx_lib_amd64_folder%\*.dll" "%wx_output_amd64_folder%"
)


:: Set ms visual studio location based on architecture
set sdk_path="C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.Cmd"
IF %processor_architecture% == x86 (
	set vs_path="C:\Program Files\Microsoft Visual Studio %version%\VC\vcvarsall.bat"
) ELSE (
	set vs_path="C:\Program Files (x86)\Microsoft Visual Studio %version%\VC\vcvarsall.bat"
)

:: Call vs batch scripts to setup dev environment
IF NOT EXIST %sdk_path% (
	IF %processor_architecture% == x86 (
		IF %arch% == x64 (
			call %vs_path% x86_amd64
		) ELSE (
			call %vs_path% x86
		)
	) ELSE (
		IF %arch% == x86 (
			call %vs_path% x86
		) ELSE (
			call %vs_path% amd64
		)
	)
) ELSE (
	IF %arch% == x86 % (
		call %sdk_path% /Release /x86 /xp
	) ELSE (
		call %sdk_path% /Release /x64 /xp
	)
)

:: Change to build directory
cd ../build

:: Remove previous vs generated files
rmdir /s /q src

:: Change to msw directory
cd msw

:: Remove all msw vs generated files
DEL *.* /s /f /q

:: Generate visual studio solutions based on user -arch
IF %arch% == x86 % (
	cmake -G "Visual Studio 10" ../../src/
) ELSE (
	cmake -G "Visual Studio 10 Win64" -D 64_BIT=true ../../src/
)

:: Generate nmake makefiles
::cmake -G "Visual Studio 10" ../../src/

:: Execute the build
msbuild.exe GMAT.sln /p:Configuration=%target% /t:Rebuild

:: Generate nmake makefiles
::cmake -G "NMake Makefiles" ../../src/

:: Build Gmat using user specified build target
::nmake SHARED=1 BUILD=release

:: Change back to build directory
cd ../

echo.
echo .........................
echo GMAT Build Complete!
echo .........................


:end

