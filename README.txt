Welcome to the wonderful world of GMAT!

GMAT is a software system for space mission design, navigation, and optimization 
applicable to missions anywhere in the solar system ranging from low Earth orbit 
to lunar, Liberation point, and deep space missions. The system contains 
high-fidelity space system models, optimization and targeting, built-in 
scripting and programming infrastructure, and customizable plots, reports and 
data products, to enable flexible analysis and solutions for custom and unique 
applications. GMAT can be driven from a fully featured, interactive Graphical 
User Interface (GUI), or from a custom script language. 

The system is implemented in ANSI standard C++ using an Object Oriented 
methodology, with a rich class structure designed to make new features easy to 
incorporate. GMAT has been used extensively as a design tool for many missions 
including LCROSS, ARTEMIS and LRO and for operational support of TESS, SOHO, 
WIND, ACE, and SDO.

-----------------------------------------------------------------------
                          License and Copyright
-----------------------------------------------------------------------

NASA Docket No. GSC-18,479-1, identified as “General Mission Analysis
Tool (GMAT) Version R2020a”

Copyright © 2020 United States Government as represented by the 
Administrator of the National Aeronautics and Space Administration. 
All Rights Reserved. 

Licensed under the Apache License, Version 2.0 (the "License"); 
you may not use this file except in compliance with the License. 
You may obtain a copy of the License at 

http://www.apache.org/licenses/LICENSE-2.0 

Unless required by applicable law or agreed to in writing, software 
distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
See the License for the specific language governing permissions and 
limitations under the License.

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
- wxWidgets 3.0.4 (http://www.wxwidgets.org/)
- TSPlot (http://sourceforge.net/projects/tsplot/)
- SPICE (http://naif.jpl.nasa.gov/naif/toolkit.html)
- SOFA (http://www.iausofa.org/)
- Apache Xerces (http://xerces.apache.org)
- JPL SPICE (https://naif.jpl.nasa.gov/naif/)  should his be here twice??
- OpenFramesInterface (https://gitlab.com/EmergentSpaceTechnologies/OpenFramesInterface)
- Boost (http://www.boost.org/)
- f2c (http://www.netlib.org/f2c)
- MSISE 1990 Density Model (https://ccmc.gsfc.nasa.gov/modelweb/atmos/msis.html)
- IRI 2007 Ionosphere Model (https://ccmc.gsfc.nasa.gov/modelweb/ionos/iri.html)

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
releases were built and tested on Ubuntu 18.04 LTS and on Red Hat 
Enterprise Linux 7.7.  Linux users on those platforms can download the 
tarball file and uncompress it into place using the command

    tar -zxf <TarballPackageName>

where <TarballPackageName> is either gmat-ubuntu-x64-R2020a.tar.gz
or gmat-rhel7-x64-R2020a.tar.gz.

UBUNTU 18.04 Note:  Ubuntu 18.04 does not ship with libpng12.so, used by
the wxWidgets libraries bundled with the Ubuntu installation.  You can 
download the needed library from the Ubuntu packages site

   https://packages.ubuntu.com/xenial/amd64/libpng12-0/download

Download the package and install it using the Ubuntu package manager.

MAC

The Mac distribution is packaged as a ZIP file, gmat-macosx-x64-R2020a.zip,
ready for expansion and use.  This prebuilt release was built and tested on 
Mac OS X 10.14 Mojave.

Mac users can download the zip archive and uncompress it into place
by double-clicking it in the Finder.

NOTE that there is a Gatekeeper/translocation issue with OS X Sierra and 
later operating systems.  Since the GMAT application is currently unsigned, 
the Gatekeeper will not allow it to run in the bin folder and will make a 
copy of it in a temporary location.  This can be fixed manually by doing 
the following:

1. Open the Finder
2. Browse to the location of the GMAT application (in the bin folder)
3. Using the Finder, drag the application only (not the entire bin 
folder!) to a different location (e.g. the Desktop)
4. Drag and drop the GMAT application back into the bin folder

This will clear the ‘UseAppTranslocation’ flag in the OS and will allow
GMAT to be run from the bin folder.

A future release of GMAT will address the signing issue further.

To use the MATLAB interface on Mac, see the additional instructions in the 
"Configuring the MATLAB Interface" section of the GMAT User Guide.

To use the Python interface on Mac, see the additional instructions in the 
"Configuring the Python Interface" section of the GMAT User Guide.

BUILDING FROM SOURCE

GMAT is distributed in source form as well, and can be compiled on OS X,
Linux and Windows. Build instructions for GMAT can be found at
http://gmatcentral.org/display/GW/GMAT+CMake+Build+System.

USING GMAT OPTIMAL CONTROL / CSALT

Using the GMAT Optimal Control capability -- implemented in the 
EMTGModels and CsaltInterface plugins -- requires additional
installation steps. See the "Software Organization and Compilation" section 
of the GMAT Optimal Control user guide in
gmat/docs/GMAT_OptimalControl_Specification.pdf for complete instructions.

-----------------------------------------------------------------------
                           Running GMAT
-----------------------------------------------------------------------

WINDOWS

On Windows 10, point to the Start menu, scroll to the GMAT folder in
the applications list, and select GMAT R2020a.

If you are using the zip-file bundle, double-click the 
bin\GMAT.exe application.

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
if MATLAB is installed in the /usr/local/MATLAB/R2019b folder and the GMAT 
console application is not loading the MATLAB plugin, you may need to 
update the library path before starting the application, like this:

export LD_LIBRARY_PATH=/usr/local/MATLAB/R2019b/bin/glnxa64:./libwx:$LD_LIBRARY_PATH 
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

   open GMAT-R2020a_Beta.app

or by double-clicking the GMAT-R2020a_Beta.app in the Finder.  If running
GMAT fails and you see an error message about GMAT not being able to 
read the startup file in a temporary location that includes 
“AppTranslocation” in the path, see the note above about the Gatekeeper and
OS X Sierra (and High Sierra).

Please see the GMAT User Guide for important instructions on how to use MATLAB,
Python, SNOPT7, and gfortran with the Mac version of GMAT.

Note for MacBooks with a Touch Bar:

The previous issue with WxWidgets, the third party GUI library
used by GMAT, and the Mac Touch Bar appears to have been fixed in recent
OSX updates.


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

Source code is available here:
https://sourceforge.net/p/gmat/git/

Compilation instructions are available here: 
http://gmatcentral.org/display/GW/GMAT+CMake+Build+System

Design Documentation is available at the links below:
http://gmatcentral.org/display/GW/Design+Documents
http://gmatcentral.org/display/GW/How+To+Write+New+Components

You can sign up for mailing lists here:
https://sourceforge.net/p/gmat/mailman/
