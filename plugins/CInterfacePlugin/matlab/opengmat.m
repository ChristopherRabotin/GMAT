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

try
    % Copy gmat_startup_file to local directory, unless it already is
    % present Note that this requires gmat_startup_file's ROOT_PATH to be
    % specified as a full absolute path (not a relative path).
    gsf = which('gmat_startup_file.txt');
    if ~strcmp(fileparts(gsf),pwd)
        copyfile(gsf,'.');
    end
    % Load the library
    if ~(libisloaded('libCInterface'))
        [notfound, warnings] = loadlibrary('libCInterface', ...
            @interfacewrapper);
        
        if (~isempty(notfound)) || (~libisloaded('libCInterface'))
            error('Unable to load the libCInterface shared library.  Warnings were: %s\n', warnings);
        end
    end
    % Start the GMAT engine
    retval = calllib('libCInterface','StartGmat');
    if retval ~= 0
        error('GMAT failed to start.  This may be caused by missing or misconfigured files; check the gmat_startup_file.\n');
    end
    msg = calllib('libCInterface','getLastMessage');
    
    disp(msg);
catch %#ok<CTCH>
    root_path = findgmatrootpath;
    curr_path = pwd;
    cd(root_path)
    opengmat
    cd(curr_path)
end

end % function
