Welcome to the wonderful world of GMAT!

GMAT is a space trajectory optimization and mission analysis system
developed by NASA and private industry in the spirit of the NASA
Vision. It is a collaboratively developed, open-source tool written to
enable the development of new mission concepts and to improve current
missions.

This release of GMAT introduces production quality builds of the core 
GMAT system for Mac and Linux workstations.  The command line GMAT
application has been tested extensively using GMAT's script based test
system, and is now certified as fully functional.  The GUI interface
for Mac and Linux users has not undergone rigorous testing, and should
be considered as a beta interface into GMAT for Mac and Linux users. 
Additional notes can be found below for the Mac and Linux release 
packages.

-----------------------------------------------------------------------
                          License and Copyright
-----------------------------------------------------------------------

Copyright (c) 2002 - 2015 United States Government as represented by
the Administrator of the National Aeronautics and Space Administration.
All Other Rights Reserved.

GMAT is licensed under the Apache License, Version 2.0 found
in the License.txt file contained in this distribution.

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
releases were built and tested on Ubuntu 14.04 LTS and on Red Hat 
Enterprise Linux 5.  Linux users on those platforms can download the 
tarball file and uncompress it into place using the command

    tar -zxf <TarballPackageName>

where <TarballPackageName> is either gmat-ubuntu-x64-R2015a.tar.gz
or gmat-rhel5-x64-R2015a.tar.gz.  


MAC

The Mac distribution is packaged as a compressed TAR file, ready for 
expansion and use.  This prebuilt release was built and tested on 
Mac OS X 10.8.  

Mac users can download the tarball file and uncompress it into place 
using the Finder or by opening a Terminal window and typing the 
command

    tar -zxf gmat-macosx-x64-R2015a.tar.gz

BUILDING FROM SOURCE

GMAT is distributed in source form as well, and can be compiled on OS X,
Linux and Windows. Build instructions for GMAT can be found at
http://gmatcentral.org/display/GW/GMAT+CMake+Build+System.



-----------------------------------------------------------------------
                           Running GMAT
-----------------------------------------------------------------------

WINDOWS

On Windows, click Start, and then point to All Programs. Point to GMAT,
GMAT R2015a, and then click GMAT R2015a. If you are using the zip-file bundle,
double-click the bin\GMAT.exe application.


LINUX

On Linux, open a terminal window and change directories to the GMAT bin 
folder.  The GMAT command line program is executed by running the GmatConsole 
application in that folder:

   ./GmatConsole

The Beta GUI can be run using the same terminal window by running Gmat_Beta:

   ./Gmat_Beta

Linux users can create a launcher for either the command line application or 
the GUI application by following the instructions for that process for their
Linux distribution.

MATLAB interface note for Linux users:

If the Matlab interface does not work with the GmatConsole command line 
application (or the Gmat_Beta GUI), you may need to set the library load 
path to include the path to your installed Matlab libraries.  For example, 
if MATLAB is installed in the /usr/local/MATLAB/R2015b folder and the GMAT 
console application is not loading the MATLAB plugin, you may need to 
update the library path before starting the application, like this:

export LD_LIBRARY_PATH=/usr/local/MATLAB/R2015b/bin/glnxa64:./libwx:$LD_LIBRARY_PATH 
./GmatConsole


MAC

On OS X, open a Terminal window and navigate to the GMAT bin folder.  The 
GMAT command line program is executed by running the GmatConsole 
application in that folder:

   ./GmatConsole

The Beta GUI application can be run using the open Terminal command, 
executed in the bin folder:

   open GMAT_Beta.app

or by double-clicking the GMAT_Beta.app in the Finder.

Python Interface note for Mac users:

Currently, GMAT will only be able to interface with Python if your Python
is installed into a specific directory: 
/Library/Frameworks/Python.framework/Versions/3.4

GMAT will be updated for a future release to allow use of a 
Python installation elsewhere.

MATLAB interface note for Mac users:

To use the Matlab interface with the Mac GMAT_Beta.app application, 
you must open the gmat_startup_file.txt and edit the MATLAB_APP_PATH field 
to point to the location of your MATLAB installation. 

If the Matlab interface does not work with the GmatConsole command line 
application, you may need to set up your Terminal so that the system can
load the Matlab libraries.  For example, if you are using a .bashrc, you
may need to add something like this:

export MATLAB = <your MATLAB app location here>
export DYLD_LIBRARY_PATH=$MATLAB/bin/maci64:$DYLD_LIBRARY_PATH


-----------------------------------------------------------------------
                           Documentation
-----------------------------------------------------------------------
GMAT is extensively documented in the docs folder of this distribution.
End-user documentation can be found in the docs\help folder (for offline
use) or online at http://gmat.sf.net/docs.

-----------------------------------------------------------------------
            HOW TO RUN THE DEMOS INCLUDED IN THIS DISTRIBUTION
-----------------------------------------------------------------------

We've included over 50 sample missions in the distribution.
The sample missions show how to apply GMAT to problems ranging from
the Hohmann transfer to Libration point stationkeeping to trajectory
optimization.

Here are instructions for running the sample missions:

  - Download and install the GMAT application.
  - Start GMAT and choose File > Open from the menu.
  - Navigate to the samples folder which is
    located in the GMAT root directory.
  - Choose the script file of your choice.
  - Click the blue "Run" button on the toolbar
    and watch the mission evolve.

NOTE: There are several samples that require enabling optional plugins.
For those samples in the NeedMatlab folder, you must enable the
libMatlabInterface plugin in the gmat_startup_file.txt (located in the bin
folder). External configuration is necessary to successfully connect
to Matlab. See the documentation for instructions. The NeedVF13ad folder
contains samples that require libVf13adOptimizer, which is not shipped
with this distribution. It is available for internal NASA users. Other
users will be able to find instructions on the forums for building and
using the VF13ad optimizer soon. Finally, those samples in the
PreliminaryEstimation folder require the libGmatEstimation plugin to be
enabled in the gmat_startup_file.txt. GMAT's estimation features in this
release are of alpha quality, and are included here as a preview only.


-----------------------------------------------------------------------
                      Credits and Acknowledgments
-----------------------------------------------------------------------
GMAT uses:
- wxWidgets 3.0.2 (http://www.wxwidgets.org/) 
- TSPlot (http://sourceforge.net/projects/tsplot/)
- SPICE (http://naif.jpl.nasa.gov/naif/toolkit.html)
- SOFA (http://www.iausofa.org/)
- Apache Xerces (http://xerces.apache.org)

Planetary images are courtesy of: 
JPL/Caltech/USGS (http://maps.jpl.nasa.gov/)
Celestia Motherlode (http://www.celestiamotherlode.net/)
Bjorn Jonsson (http://www.mmedia.is/~bjj/)
NASA World Wind (http://worldwind.arc.nasa.gov/)

Some icons are courtesy of Mark James ( http://www.famfamfam.com/ )


-----------------------------------------------------------------------
                           Contact Information
-----------------------------------------------------------------------

For general project info see: http://gmatcentral.org

For source code and application distributions see:
http://sourceforge.net/projects/gmat/

For other comments and questions, email: gmat@gsfc.nasa.gov


-----------------------------------------------------------------------
                         Known Issues and Status
-----------------------------------------------------------------------

NOTES ON THE LINUX AND MAC RELEASES

GMAT R2015a is the first non-beta release of GMAT on Mac and Linux 
workstations.  The release on those platforms is production quality for 
the command line application, GmatConsole, and beta quality for the GMAT
graphical user interface, GMAT, which has been renamed Gmat_Beta.

The following plugin modules do not run under this release of GMAT on
Mac and Linux platforms:

  libFminconOptimizer
  libMarsGRAM

In addition, the RHEL5 build of GMAT does not run the following plugin
modules:

  libMatlabInterface
  libPythonInterface

and the Mac release does not support the following plugin:
 
  libMsise86

The GMAT console application provides the interface into the validated 
GMAT numerical engine.  There are two options for the console application 
that are not recommended for use in this release: the --save option and
the --verbose option.


NOTES ON THE R2015a RELEASE 

Please see the release notes, located online at http://gmat.sf.net/docs.

For a list of known issues, please visit
http://bugs.gmatcentral.org or email gmat@gsfc.nasa.gov.

The following plugins are included and have the listed status:

Official plugins:
  libDataInterface
  libEphemPropagator
  libEventLocator
  libFminconOptimizer
  libFormation
  libGmatFunction
  libMatlabInterface
  libNewParameters
  libProductionPropagators
  libPythonInterface
  libStation

Alpha plugins (disabled by default):
  libCInterface
  libExtraPropagators
  libGeometricMeasurements
  libGmatEstimation
  libPolyhedronGravity
  libSaveCommand

Internal-only plugins (disabled for public releases):
  libMarsGRAM
  libMsise86
  libNRLMsise00
  libSNOptimizer
  libVF13Optimizer
