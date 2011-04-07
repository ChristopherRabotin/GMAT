-----------------------------------------------------------------
Prerequisites
-----------------------------------------------------------------
To generate the GMAT help documentation, the following tools must be installed
and present on the system path for the current user:

* Java

* GNU Make (http://gnuwin32.sourceforge.net/packages/make.htm)
  or
  Microsoft NMAKE (bundled with Visual Studio)

* Subversion command-line client
    Download from http://www.collab.net or http://www.sliksvn.com

* Zip
	Download: ftp://ftp.info-zip.org/pub/infozip/win32/

* HTML Help Workshop (for Windows Help output)
    Download: http://msdn.microsoft.com/en-us/library/ms669985%28v=VS.85%29.aspx


-----------------------------------------------------------------
Usage
-----------------------------------------------------------------
Windows:
    1. Open a command window (such as cmd.exe).
    2. Navigate to the the folder containing makedocs.cmd.
    3. Run "makedocs". All output files will be placed in the build
        directory, and the final files will be moved to the output directory.
        
Mac/Linux:
    1. Open a shell.
    2. Navigate to the folder containing makedocs.
    3. Run "./makedocs" or "sh makedocs".