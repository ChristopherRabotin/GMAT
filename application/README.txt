
This document and associated files\folders pertain to the GMAT 
Beta(Windows) and Alpha (Mac,Linux) versions that are based off 
of the Sep. 11 2008 source code.

This build HAS NOT been thoroughly GUI system tested to ensure increased
stability. Regression tests have been run at regular intervals and show 
that the numerical results of the 12/10/2007 build is comparable to 
the current release.

-----------------------------------------------------------------------
                          License and Copyright
-----------------------------------------------------------------------

Copyright 2002-2008 United States Government as represented by
the Administrator of The National Aeronautics and Space
Administration. All Rights Reserved.

GMAT is licensed under the NASA Open Source Agreement found in the
License.txt file contained in this distribution

-----------------------------------------------------------------------
                           What's New??
-----------------------------------------------------------------------
-  68 bug fixes (detailed below)
-  GMAT Functions capabilities
-  Object Plug-in capabilities
-  Mac Intel support
-  New ParameterSelectDialog GUI
-  Improved error messages in script syntax checking

-----------------------------------------------------------------------
                      Installation and Configuration
-----------------------------------------------------------------------

The GMAT windows distribution contains an installer that will
install and configure GMAT for you automatically.  After installation
is completed, if you have MATLAB®, add all folders under the "matlab"
folder in the GMAT root directory, to your MATLAB® path.

For installation under Linux and Mac, please read the INSTALL file
included in the distribution.  

If you have MATLAB® installed on your machine, use the executable
named GMAT*DLL.  If you do not have MATLAB®, use the executable
named GMAT*DLLNoMatlab.  MATLAB® is not fully supported in the Mac 
and Linux releases.

-----------------------------------------------------------------------
            HOW TO RUN THE DEMOS INCLUDED IN THIS DISTRIBUTION
-----------------------------------------------------------------------

We’ve included more than 20 sample missions in the distribution.
The sample missions show how to apply GMAT to problems ranging from
the Hohmann transfer to Libration point stationkeeping to trajectory
optimization.

Here are instructions for running the sample missions:

  - Download and install the GMAT application.
  - Open GMAT and left click on the yellow folder
    icon on the toolbar.
  - Navigate to the Sample Missions folder which is
    located in the GMAT root directory.
  - Select a script file of your choice and hit ok.
  - Hit the green run button located in the toolbar,
    and watch the mission evolve.

NOTE:  To run optimization missions, you need MATLAB®, and the
MATLAB® optimization toolbox, and must use the Windows GMATBeta executable.
MATLAB® is not fully supported in the Mac and Linux
releases and therefore you cannot utilize GMAT's optimization 
capabilities on the current Mac and Linux builds.

-----------------------------------------------------------------------
                      Credits and Acknowledgments
-----------------------------------------------------------------------
GMAT uses WxWidgets 2.8.8 (http://www.wxwidgets.org/) 
GMAT uses TSPlot (http://sourceforge.net/projects/tsplot/)

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

For a known issues list, please visit http://pows003.gsfc.nasa.gov/bugzilla/
or email gmat@gsfc.nasa.gov

This release of GMAT is a snapshot of the system. While GMAT has undergone
extensive testing and is mature software, we consider the system to be in 
Beta form on some platforms and Alpha form on others. The GMAT team
considers the system to be in an alpha release state as long as the
system is functional, but has not been rigorously tested for
numerical precision and functional stability. Once testing has been
performed and stability verified, the system becomes a beta product
if the number of open issues is considered small and the system is
usable for most problems. GMAT has the following status on the
supported Platforms:

Windows: Beta version. Mac: Alpha version. Linux: Alpha version.

We are in the process of performing significant testing to prepare
GMAT for operational use. GMAT's numerics and models have received
extensive testing against operational systems. The dynamics models,
integrators, propagators, coordinate systems, and parameter
calculations meet the project's acceptance criteria, with only a few
exceptions. Please see the Acceptance Test Plan to learn more about
the few cases where numerical issues still exist. For a detailed
list of existing bugs and issues, please visit the project's Bug
Tracker.

Critical Numerical Bugs
=======================

-- Step mode propagation vs. straight propagation (#207)
For our test cases with SMAs larger than 400,000 km (DeepSpace, EML2, ESL2),
the results are different for propagating inside of a for loop and 
propagating outside of any control flow.

--Some nested targeter loops numerically different (#585)
In a test case with several different targeting loop runs, with the
expectation that the output of each run should be identical, there are
a few runs with numerical differences on the order of 10e-7 or smaller.

--Quirky Numerical Issues (#1011, 1012)
When first running the GMAT executable, the results are slightly different
than those of each subsequent GMAT executable run. For this bug an executable
run consists of opening GMAT and closing it when finished running test scripts.

-----------------------------------------------------------------------
               Bugs Fixed Since Last Release (12-10-2007)
-----------------------------------------------------------------------

29	GMAT application - doesn't work for another machine
48	Propagation Accuracy 1 - SRP force at other bodies
121	Report file - Enable multiple selections
161	Quit (under GMAT menu) does not work properly
297	OpenGL enhancement 7 - Modify OpenGL view menu
397	file text box not working for script folder execution
399	Animation Inifinite Loop
405	Crashes when focus on other program
417	Earth Sun Lines on OpenGL plot
479	Message Panel can Consume Entire Window
480	Tree panel can vanish
581	Initial Show Script does not include displayed values
586	Earth Sun Line Bug
609	File filters are too restrictive
641	Can't delete objects that are named using only one or two letters
775	Spacecraft range bug
915	Inconsistency between Toggle and Save panels
925	Some user defined coordinate systems cannot be deleted
937	Manuever Panel input field changes automatically
944	Rename fails for MATLAB Function Panel
948	XYPlot Setup Panel
977	Parameter Select Dialog Redesign
1058	OpenGL plot disappears... never to return again
1066	Multiple OpenGL plots crash GMAT on Linux
1098	Propagator Central Body Changes from Sun to Earth on Save
1101	crashes every other run
1123	DC Does not converge in GMAT, but does in other systems
1130	Clicking Close All Buttons Removes All Reports from Output Tree
1132	Error in script with no error displayed in msg window
1144	OpenGL Plot not saving ViewPointVector settings correctly
1145	Saved mission has syntax error in Propagator configuration
1147	Comments are not saved in Script Events
1150	Report command ReportFile field not updating
1151	String variable not showing up in ParameterSelectDialog
1152	ReportFile object not displaying content from script
1158	Crash: Validation script crashes on load
1160	Crash when switching to Resource Tree While Vay Panel is Open
1164	Can't set values of arrays and strings, or close dialog box.
1165	MATLAB Engine not working
1171	Invisible button right of Help on Play
1172	Text wrap should be off in Begin/EndScript
1175	Can not use Save command on certain objects
1176	Can't set Fuel Tanks equal to one another
1177	Crash when deleting the Propagate command from Default Project
1178	Deleting Commands shows unnecessary attitude message
1180	Matlab Server: Asterisk stripped from lines passed to GMAT
1182	Some objects cannot be copied in command mode
1186	Crash: Using Matlab to GMAT interface
1188	Tab After End of Line Causes Error
1189	Crash: Loading Default Project after Error in Loaded Script
1200	Crash: CS Origin is Spacecraft and Axes is BodyFixed
1202	Icons for Vary and Achieve Events are Swapped In GUI
1215	Quotes not stripped when setting an object to a string variable
1242	Typo in error message for NonlinearConstraint
1251	VF13ad fails to converge and gives confusing error message
1254	Creating String and Report with Same Names Causes Crash
1262	Report parameters disappear from GUI
1268	Save from GUI Loses Entire Mission Sequence!!!
1277	Object Referenced CoordSys Spacecraft Error
1282	problem with XY plots using variables.
1284	Crash due to space in Create Spacecraft line
1285	variable expressions don't work
1286	faulty XY plots
1295	Error changing planetary ephemeris source to SLP
1298	Crash on nested GMAT functions
1299	GMAT Function Crash on Function Input Mismatch
1300	GMAT Function crash: Issue with Open If Control Flow
1304	Rename + Cancel = Incorrect Message
