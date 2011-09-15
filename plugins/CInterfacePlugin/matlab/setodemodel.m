function [retval] = setodemodel(modelid)

% SETODEMODEL GMAT force model accessor
%  findodemodel(modelname) Sets the active ODE model to a previously
%  located model
%
%   INPUTS
%   VARIABLE        SIZE    	DESCRIPTION (Optional/Default)
%   modelid         (1x1)       The integer ID for the model.
%
%   OUTPUTS
%   retval          (1x1)       The ID for the current Model.
%
%   keyword: GMAT Propagator Forces
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
% Created: 2011/08/30

% Load the library
if ~(libisloaded('libCInterface'))
    error('Please load the GMAT interface first.');
end

% Prepare configuration script options
if (nargin >= 1)
    if (modelid < 1000) && (modelid ~= 0)
        error('ODE model IDs start at 1000, or use 0 for the first model in the list');
    end
    if modelid == 0
        modelid = 1000;
    end
else
    modelid = 1000;
end

% Set the ODE model to the one named in the script (currently uses the
% first model found; will use named models later on)
retval = calllib('libCInterface','SetModel',modelid);
if retval < 0
    error('Unable to locate the ODE model with id %d.\n', modelid);
end

%if verbosity == 1
%    msg = calllib('libCInterface','getLastMessage');
%    disp(msg);
%end

end     % function
