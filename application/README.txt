Welcome to the wonderful world of GMAT!

GMAT is a space trajectory optimization, navigation, and mission 
analysis system developed by NASA and private industry in the spirit 
of the NASA Vision. It is a collaboratively developed, open-source 
tool written to enable the development of new mission concepts and
to improve current missions.

-----------------------------------------------------------------------
                          License and Copyright
-----------------------------------------------------------------------

Copyright (c) 2002 - 2017 United States Government as represented by
the Administrator of the National Aeronautics and Space Administration.
All Other Rights Reserved.

GMAT is licensed under the Apache License, Version 2.0 found
in the License.txt file contained in this distribution.

-----------------------------------------------------------------------
                           Contact Information
-----------------------------------------------------------------------

For general project info see: http://gmatcentral.org

For source code and application distributions see:
http://sourceforge.net/projects/gmat/

For other comments and questions, email: gmat@gsfc.nasa.gov

-----------------------------------------------------------------------
                      Credits and Acknowledgments
-----------------------------------------------------------------------
GMAT uses:
- wxWidgets 3.0.2 (http://www.wxwidgets.org/) 
- TSPlot (http://sourceforge.net/projects/tsplot/)
- SPICE (http://naif.jpl.nasa.gov/naif/toolkit.html)
- SOFA (http://www.iausofa.org/)
- Apache Xerces (http://xerces.apache.org)
- JPL SPICE (https://naif.jpl.nasa.gov/naif/)
- Boost (http://www.boost.org/)

Planetary images are courtesy of: 
- JPL/Caltech/USGS (http://maps.jpl.nasa.gov/)
- Celestia Motherlode (http://www.celestiamotherlode.net/)
- Bjorn Jonsson (http://www.mmedia.is/~bjj/)
- NASA World Wind (http://worldwind.arc.nasa.gov/)

Some icons are courtesy of Mark James ( http://www.famfamfam.com/ )

-----------------------------------------------------------------------
                      Installation and Configuration
-----------------------------------------------------------------------

WINDOWS

The GMAT Windows distribution contains an installer that will
install and configure GMAT for you automatically. The installer
does not require administrative privileges, and will install by
default to your personal %LOCALAPPDATA% folder. This can be customized
during installation.

LINUX

There are two precompiled Linux distributions of GMAT, packaged as 
compressed TAR files, ready for expansion and use.  The precompiled 
releases were built and tested on Ubuntu 16.04 LTS and on Red Hat 
Enterprise Linux 7.2.  Linux users on those platforms can download the 
tarball file and uncompress it into place using the command

    tar -zxf <TarballPackageName>

where <TarballPackageName> is either gmat-ubuntu-x64-R2017a.tar.gz
or gmat-rhel7-x64-R2017a.tar.gz.  

MAC

The Mac distribution is packaged as a ZIP file, ready for
expansion and use.  This prebuilt release was built and tested on 
Mac OS X 10.11.  

Mac users can download the zip archive and uncompress it into place
by double-clicking it in the Finder or by opening a Terminal window
and typing the command

    unzip gmat-macosx-x64-R2017a.zip

NOTE that there is a Gatekeeper/translocation issue with OS X Sierra.
Since the GMAT application is currently unsigned, the Gatekeeper will not
allow it to run in the bin folder and will make a copy of it in a 
temporary location.  This can be fixed manually by doing the following:

1. Open the Finder
2. Browse to the location of the GMAT application (in the bin folder)
3. Using the Finder, drag the application only (not the entire bin 
folder!) to a different location (e.g. the Desktop)
4. Drag and drop the GMAT application back into the bin folder

This will clear the ‘UseAppTranslocation’ flag in Sierra and will allow
GMAT to be run from the bin folder.

A future release of GMAT will address the signing issue further.

BUILDING FROM SOURCE

GMAT is distributed in source form as well, and can be compiled on OS X,
Linux and Windows. Build instructions for GMAT can be found at
http://gmatcentral.org/display/GW/GMAT+CMake+Build+System.

-----------------------------------------------------------------------
                           Running GMAT
-----------------------------------------------------------------------

WINDOWS

On Windows, click Start, and then point to All Programs. Point to GMAT,
GMAT R2017a, and then click GMAT R2017a. If you are using the zip-file
bundle, double-click the bin\GMAT.exe application.

LINUX

On Linux, open a terminal window and change directories to the GMAT bin 
folder.  The GMAT command line program is executed by running the 
GmatConsole application in that folder:

   ./GmatConsole

The Beta GUI can be run using the same terminal window by running 
Gmat_Beta:

   ./GMAT_Beta

Linux users can create a launcher for either the command line 
application or the GUI application by following the instructions for 
that process for their Linux distribution.

MATLAB interface note for Linux users:

If the Matlab interface does not work with the GmatConsole command line 
application (or the GMAT_Beta GUI), you may need to set the library load 
path to include the path to your installed Matlab libraries.  For example, 
if MATLAB is installed in the /usr/local/MATLAB/R2016b folder and the GMAT 
console application is not loading the MATLAB plugin, you may need to 
update the library path before starting the application, like this:

export LD_LIBRARY_PATH=/usr/local/MATLAB/R2016b/bin/glnxa64:./libwx:$LD_LIBRARY_PATH 
./GmatConsole

Finally, the MATLAB engine used in the Linux release runs under the C shell, 
csh.  If you plan to use the MATLAB interface, make sure that you have csh
installed on your Linux machine.

MAC

On OS X, open a Terminal window and navigate to the GMAT bin folder.  The 
GMAT command line program is executed by running the GmatConsole 
application in that folder:

   ./GmatConsole

The Beta GUI application can be run using the ‘open’ Terminal command, 
executed in the bin folder:

   open GMAT-R2017a_Beta.app

or by double-clicking the GMAT-R2017a_Beta.app in the Finder.  If running
GMAT fails and you see an error message about GMAT not being able to 
read the startup file in a temporary location that includes 
“AppTranslocation” in the path, see the note above about OS X Sierra and 
the Gatekeeper.

Python Interface note for Mac users:

Currently, GMAT will only be able to interface with Python if your Python 
is installed into a specific directory: 
/Library/Frameworks/Python.framework/Versions/3.6 

GMAT will be updated for a future release to allow use of a 
Python installation elsewhere.

MATLAB interface note for Mac users:

To use the Matlab interface with the Mac GMAT-R2017a_Beta.app application, 
you must open the MATLABConfigure.txt in the bin directory and edit the 
MATLAB_APP_PATH field to point to the location of your MATLAB installation. 

If the Matlab interface does not work with the GmatConsole command line 
application, you may need to set up your Terminal so that the system can
load the Matlab libraries and start up MATLAB.  For example, if you 
are using a .bashrc, you may need to add something like this:

export MATLAB = <your MATLAB app location here>
export DYLD_LIBRARY_PATH=$MATLAB/bin/maci64:$DYLD_LIBRARY_PATH
export PATH=$PATH:$MATLAB/bin

-----------------------------------------------------------------------
                           User Information
-----------------------------------------------------------------------

User docs are available in pdf and html format.  The pdf documentation
is distributed in letter and A4 size in this package.  The files are
located here: /doc/help/help-letter.pdf and /doc/help/ help-a4.pdf
Online documentation is available here: http://gmat.sf.net/docs.

For new users, see the Getting Started and Tour of GMAT sections first,
then take the tutorials.  The tutorials are included in print versions
in the help documents, and are available in video form here:
https://www.youtube.com/channel/UCt-REODJNr2mB3t-xH6kbjg

For a list of new functionality, compatibility changes, and known issues,
see the Release Notes section in the user guide.  

-----------------------------------------------------------------------
                         Developer Information
----------------------------------------------------------------------

For a complete list of changes made in this version see the Release 
Notes section of the user guide.

Source code is available here:
https://sourceforge.net/p/gmat/git/ci/GMAT-R2016a/tree/

Compilation instructions are available here: 
http://gmatcentral.org/display/GW/GMAT+CMake+Build+System

Design Documentation is available at the links below:
http://gmatcentral.org/display/GW/Design+Documents
http://gmatcentral.org/display/GW/How+To+Write+New+Components

You can sign up for mailing lists here:
https://sourceforge.net/p/gmat/mailman/
