% ******************************************************************************
% 
% MATLAB Solar System Ephemeris Toolbox GUI                        17 April 2001
%                                              A. La Bute , Kennedy Space Center
% 
% ******************************************************************************
% 
% INTRODUCTION
% ------------
% 
% This gui_readme file describes how to use the Matlab Solar System Ephemeris
% Toolbox GUI. For more information on the actual functions in the toolbox please
% refer to the README file.
% 
% 
% CREATING FILES
% To create a new binary file, select the option "Create New File" from the main
% menu.  Two windows will appear: a new Matlab GUI and a web browser.  Use the 
% web browser to download both the ascii file(s) and the header file to your current
% directory.  Copy the names of both the ascii file and the header file to the 
% GUI.  Select a name for the binary file that will be created.  Press "Done".
% 


% TESTING FILES
% The Test File GUI appears twice.  It will appear after a new binary file has 
% been created, and it can be activated by the user in the Use File GUI.  When
% activated, two windows will appear: the Test File GUI and a web browser.  Use
% the web browser to download the test file to your current directory.  Copy the 
% name of the test file to the GUI.  Press "Test".

% USING FILES
% To use binary files that have already been created by Matlab, select the option
% "Use Existing File".  The Use File GUI will appear.  You must select the following:
% 

% 1)  Binary file in the current directory
% 2)  Body in the solar system, or nutations, or librations
% 3)  Body of origin for both velocity and position
% 4)  Date
% 5)  Units
% 
% Now you can retrieve the position and velocity of the desired body by pressing,
% "Get State Vector".