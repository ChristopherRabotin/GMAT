function [retval] = opengmat

% OPENGMAT GMAT engine startup routine
%  [retval] = opengmat() Loads the GMAT interface and core library 
%
%   OUTPUTS
%   retval          (1x1)       Integer return code indicating the status
%                               of the GMAT startup process.
%
%   keyword: GMAT 
%
%
% (This file is part of GMAT, The General Mission Analysis Tool, and is
%  distributed under the NASA Open Source Agreement.  See file source for
%  more details.)
%
% See also: closegmat

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
% Modified: 2011/09/08 Russell Carpenter, NASA GSFC
% Modified: 2012/11/28 Ravi Mathur, Emergent Space Technologies, Inc.

% Note that this requires gmat_startup_file's ROOT_PATH to be
% specified as a full absolute path (not a relative path).

% Find GMAT startup file in existing MATLAB paths
gsf = which('gmat_startup_file.txt');

% If startup file doesn't exist, then exit with error
if isempty(gsf)
    error('gmat_startup_file.txt does not exist in any MATLAB path. Check your paths.');
end

% Check that ROOT_PATH of startup file is a valid GMAT installation
root_path = findgmatrootpath;
if ~exist(fullfile(root_path, 'bin', 'gmat_startup_file.txt'))
    error('The ROOT_PATH variable in gmat_startup_file.txt does not point to a valid GMAT installation.');
end
    
% Copy startup file to local directory if needed
copygsf = ~strcmp(fileparts(gsf), pwd);
if copygsf
    disp('Local GMAT startup file missing ... copying global GMAT startup file:');
    disp(gsf);
    copyfile(gsf,'.');
end
    
% Load the CInterface library if needed
if ~(libisloaded('libCInterface'))
    [notfound, warnings] = loadlibrary('libCInterface', @interfacewrapper);
    
    if (~isempty(notfound) || ~libisloaded('libCInterface'))
        if(copygsf)
            delete('gmat_startup_file.txt'); % Delete if it was created
        end
        unloadlibrary('libCInterface');
        error('Unable to load the libCInterface library. Warnings were: %s\n', warnings);
    end
end

% Start the GMAT engine
retval = calllib('libCInterface', 'StartGmat');
msg = calllib('libCInterface', 'getLastMessage');
if retval ~= 0
    if(copygsf)
        delete('gmat_startup_file.txt'); % Delete if it was created
    end
    unloadlibrary('libCInterface');
    disp(['Error from GMAT: ', msg]);
    error('GMAT failed to start.  This may be caused by missing or misconfigured files; check gmat_startup_file.txt.\n');
end

% Success!
disp(['Message from GMAT: ', msg]);
    
end % function
