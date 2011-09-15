function [retval] = startgmat(filename)

% STARTGMAT GMAT engine initialization routine
%  [retval] = startgmat() Loads the GMAT interface and core library, 
%  initializes the objects in GMAT using the default script
%  (defaultOdtbx.script), populates GMAT's sandbox with the configuration
%  described in the script, runs the script, forcing GMAT to establish all
%  object-to-object interconnections, and then locates the first ODE model 
%  in the Mission Control Sequence for use by ODTBX.
%
%  [retval] = startgmat(filename) Loads the GMAT interface and core 
%  library, initializes the objects in GMAT using the specified script, 
%  populates GMAT's sandbox with the configuration described in the script, 
%  runs the script, forcing GMAT to establish all object-to-object 
%  interconnections, and then locates the first ODE model in the Mission 
%  Control Sequence for use by ODTBX.
% 
%
%   INPUTS
%   VARIABLE        SIZE    	DESCRIPTION (Optional/Default)
%   filename        (1x1)       (Optional) GMAT script used to configure
%                               and populate the GMAT sandbox.
%
%   OUTPUTS
%   retval          (1x1)       Integer return code indicating the status
%                               of the GMAT startup process.
%
%   keyword: GMAT Integrators Forces 
%
%
% (This file is part of GMAT, The General Mission Analysis Tool, and is
%  distributed under the NASA Open Source Agreement.  See file source for
%  more details.)

% GMAT: General Mission Analysis Tool
%
% Copyright (c) 2002-2011 United States Government as represented by the
% Administrator of The National Aeronautics and Space Administration.
% All Other Rights Reserved.
%
% Developed jointly by NASA/GSFC and Thinking Systems, Inc. under NASA
% Prime Contract NNG10CP02C, Task Order 28.
%
% Author: Darrel J. Conway, Thinking Systems, Inc.
% Created: 2011/05/17
% Modified: 2011/09/07, Russell Carpenter, NASA GSFC

% Prepare configuration script options
if (nargin >= 1)
    if strcmp(filename, '') == 1
        filename = 'GmatConfig.script';
    end
else
    filename = 'GmatConfig.script';
end

if CheckForFile(filename) == 0
    error('The GMAT configuration script does not exist, so GMAT cannot be loaded');
end


% Load the library and start the GMAT engine
opengmat;

% Load and populate a configuration
retval = calllib('libCInterface', 'LoadScript', filename);
if retval ~= 0
    error('The configuration script "%s" failed to load\n', filename); 
end
retval = calllib('libCInterface','RunScript');
if retval ~= 0
    error('The configuration script "%s" failed to run\n', filename); 
end

% Show the run status
ode = calllib('libCInterface','getLastMessage');
disp(ode);

% Set the ODE model to the one named in the script (uses the
% first model found)
findodemodel;

ode = calllib('libCInterface','getLastMessage');
disp(ode);

end %function
