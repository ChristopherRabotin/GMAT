Using the GMAT API
==================

Last Updated:        March 2020
Current API Status:  Beta Release

Starting with GMAT R2020a, the GMAT system includes an API providing access to 
many components of the system for users working in Python and MATLAB/Java 
environments.  The GMAT API is built with the system and included in the bundled
releases.  

The files in this folder demonstrate how to use the GMAT API.  The API User's 
Guide, in the GMAT documentation folder, includes a tutorial and  walk through 
based on these files.  The Jupyter folder included in this folder contains 
Python Jupyter notebooks that provide similar instruction for users that have 
the Jupyter server components on their systems.


Setting Up the Python API
------------------

The API Python examples are built to run from any place on the user's system 
after performing the following configuration steps:

1. Open a terminal (command prompt on Windows)
2. Change directories to the GMAT api folder
3. Execute the following command:
   
   <python> BuildApiStartupFile.py

   where <python> is the cammand used to run you Python interpreter.  This may 
   be "python", "python3", "python3.7", or some similar command.

   After running this command, you'll find a GMAT startup file named 
   "api_startup_file.txt" in the GMAT bin folder.  That startup file replaces 
   GMAT's relative path settings in you GMAT startup file with absolute path 
   settings.  This file, together with the load_gmat.py file in the api folder, 
   simplifies execution of the GMAT API from any folder on your workstation.

4. Open the load_gmat.py script, and replace the GmatInstall setting with the 
   path to your GMAT top level folder.


Setting Up the MATLAB API
------------------

The API MATLAB examples are built to run from any place on the user's system 
after adding the bin folder from your GMAT installation to your MATLAB path. 
This can be done by selecting "Set Path" from the Home ribbon at the top of 
the main MATLAB window. Then select "Add Folder..." then browse to the bin 
folder, then select that folder.


Project Status
--------------

The GMAT API is currently a beta release.  That means the system is considered 
basically stable and tested for the cases described in the API User's Guide, and
that is exposes many additional capabilities that are not yet tested.  The API
exposes many of GMAT's core objects for use from Python and MATLAB.  

The release has been tested in the following environments:

   Windows:  Using Python 3.7 and MATLAB R2019b

   Linux:    * Using Python 3.6 and MATLAB R2019b
             + Using Intel Python 3.6 and MATLAB R2019b
             * Using Anaconda Python 3.7 and MATLAB R2019

             * Both Ubuntu 18.04 and Red Hat 7
             + Ubuntu 18.04 only

   Mac:      * Using Python 3.7

             * MacOS 10.15 (Catalina)
