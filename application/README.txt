Welcome to the wonderful world of GMAT!

GMAT is a space trajectory optimization and mission analysis system
developed by NASA and private industry in the spirit of the NASA
Vision. It is a collaboratively developed, open-source tool written to
enable the development of new mission concepts and to improve current
missions.

-----------------------------------------------------------------------
                          License and Copyright
-----------------------------------------------------------------------

Copyright (c) 2002 - 2011 United States Government as represented by
the Administrator of the National Aeronautics and Space Administration.
All Other Rights Reserved.

GMAT is licensed under the NASA Open Source Agreement version 1.3 found
in the License.txt file contained in this distribution.

-----------------------------------------------------------------------
                      Installation and Configuration
-----------------------------------------------------------------------

The GMAT Windows distribution contains an installer that will
install and configure GMAT for you automatically. The installer
does not require administrative privileges, and will install by
default to your personal AppData\Local folder. This can be customized
during installation.

For installation under Linux and Mac, please read the install_linux.txt
and install_mac_snowleopard.txt files included in the distribution.

-----------------------------------------------------------------------
                           Running GMAT
-----------------------------------------------------------------------
On Windows, GMAT can be started by running bin\GMAT.exe.

On Mac and Linux, please consult install_linux.txt and
install_mac_snowleopard.txt.

-----------------------------------------------------------------------
                           Documentation
-----------------------------------------------------------------------
GMAT is extensively documented in the docs folder of this distribution.
End-user documentation can be found in the docs/help folder (for offline
use) or online at http://gmat.sf.net/docs.

-----------------------------------------------------------------------
            HOW TO RUN THE DEMOS INCLUDED IN THIS DISTRIBUTION
-----------------------------------------------------------------------

We've included nearly 40 sample missions in the distribution.
The sample missions show how to apply GMAT to problems ranging from
the Hohmann transfer to Libration point stationkeeping to trajectory
optimization.

Here are instructions for running the sample missions:

  - Download and install the GMAT application.
  - Open GMAT and choose File > Open from the menu.
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
- WxWidgets 2.8.11 (http://www.wxwidgets.org/) 
- TSPlot (http://sourceforge.net/projects/tsplot/)
- SPICE (http://naif.jpl.nasa.gov/naif/toolkit.html)
- PCRE (http://www.pcre.org/)

Planetary images courtesy of: 
JPL/Caltech/USGS (http://maps.jpl.nasa.gov/)
Celestial Motherlode (http://www.celestiamotherlode.net/)
Bjorn Jonsson (http://www.mmedia.is/~bjj/)
NASA World Wind (http://worldwind.arc.nasa.gov/)

Some icons are courtesy of Mark James ( http://www.famfamfam.com/ )

-----------------------------------------------------------------------
                           Contact Information
-----------------------------------------------------------------------

For general project info see: http://gmat.gsfc.nasa.gov

For source code and application distributions see:
http://sourceforge.net/projects/gmat/

For other comments and questions email:  gmat@gsfc.nasa.gov

-----------------------------------------------------------------------
                         Known Issues and Status
-----------------------------------------------------------------------

Please see the release notes, located online at http://gmat.sf.net/docs.

For a list of known issues, please visit http://pows003.gsfc.nasa.gov/bugzilla/
or email gmat@gsfc.nasa.gov. 

GMAT R2011a is released as beta for Windows, and as alpha for Linux and Mac.

The included plugins are disabled by default, but have the following status:
libMatlabInterface: beta
libFminconOptimizer: beta (available for Windows only)
libGmatEstimation: alpha