This folder contains files that build GMAT's Windows installer and .zip bundles.
The current installer is based on NSIS (http://nsis.sf.net), but a new installer
based on MSI (using WiX) is a long-term goal.


Prerequisites
-------------
To build the installer, you need:

1. MSYS (http://sf.net/projects/mingw)
2. zip: in MSYS, run "mingw-get msys-zip"
2. Subversion command-line tools (svn.exe) in your system path
3. NSIS in your system path


Configuration
-------------
The gmat.nsi file contains installer properties like the output filename and
project URLs.

The ../assemblegmat.sh script actually assembles the active copy of GMAT. That
file should be checked to make sure it is fetching all the correct files.


Building the installer
----------------------
From the MinGW/MSYS shell, run:

make VERSION=<version string>

This will assemble the latest copy of GMAT and build an installer file and .zip
bundles from it.