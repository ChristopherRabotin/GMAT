GMAT Dependency Configuration README
---

This readme file provides instructions for configuring the gmat library dependencies.


AUTOMATIC CONFIGURATION

The configuration script "configure.bat" for windows and "configure.sh" for Linux/Mac will take care of all dependency downloads and dependency configurations. In order to use these scripts you must have the proper prerequisite software installed.

The required software is as follows.

All OS's:

	Wget
	Subversion

Linux:

	gnome-core-devel 
	build-essential 
	libgtk2.0-dev 
	libgtk2.0-doc 
	devhelp 
	libgl1-mesa-dev 
	libglu1-mesa-dev 
	freeglut3 
	freeglut3-dev 



Mac (Lion & Mountain Lion):

	

	XCode 4.3
	Command Line Tools for XCode (Distro Dependent)

Windows:

	7Zip
	Visual Studio or Windows SDK


In order to run either script you should first open up a terminal window and navigate to the /gmat/depends/ folder. The configure.sh and configure.bat usage is as follows.

Linux:

	./configure.sh -p [ /path/to/gmat/folder/ ]

Mac: 

	./configure.sh -p [ /path/to/gmat/folder/ ] -mac

Windows:

	configure.bat -p [ /path/to/gmat/folder/ ] 
		      -wget [ /path/to/wget/wget.exe ] 
		      -svn [/path/to/svn/svn.exe ] 
		      -sevenz [ /path/to/7zip/folder/ ] 
		      -vsversion [vsversion ie. 9 | 10 | 11 ] 
		      -vspath [ /path/to/visual/studio/or/windows/sdk/ ] 
		      -latest [ true | false ]
	

If you do not wish to enter the arguments on the command line simply modify the "Default Variables" section of the configure script and run the script without them.

Please Note: You must be connected to the internet in for for these scripts to properly work.


MANUAL CONFIGURATION

For manual configuration please consult the build system documentation listed under Additional Support.


ADDITIONAL SUPPORT

For additional help configuring the GMAT Dependencies, please visit the Build System documentation on the GMAT Wiki.

	GMAT Wiki Pages:  http://gmat.ed-pages.com/wiki/tiki-index.php