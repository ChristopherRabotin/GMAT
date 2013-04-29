Overview
--------
GMAT has two parallel build systems:

* Primary: VS2010 on Windows, GCC on Mac/Linux
* Alpha: CMake on all platforms


Primary Build System
--------------------
Details on the primary build system are available on the GMAT wiki:
http://li394-117.members.linode.com:8090/display/GW/Compiling


Alpha Build System
------------------
This readme file provides instructions for Building Gmat.


AUTOMATIC BUILD

The build script "build.bat" for windows and "build.sh" for Linux/Mac will take care of all cmake make file generations and gmat compiliation. In order to use these scripts you must have the proper prerequisite software installed.

The required software is as follows.

All OS's:

	CMake


In order to run either script you should first open up a terminal window and navigate to the /gmat/build/ folder. The build.sh and build.bat usage is as follows.

Linux:

	./build.sh -arch [x86 | x64]

Mac:

	./build.sh -arch [x86 | x64] -mac


Windows:

	build.bat -arch [x86 | x64] 
		  -target [debug | release] 
		  -version [10.0 | 11.0)


If you do not wish to enter the arguments on the command line simply modify the "Default Variables" section of the build script and run the script without them.


MANUAL BUILD

For manual building of gmat please consult the build system documentation listed under Addtional Support.


ADDITIONAL SUPPORT

For additional help Building GMAT please visit the Build System documenation on the GMAT Wiki.

	GMAT Wiki Pages:  http://gmat.ed-pages.com/wiki/tiki-index.php