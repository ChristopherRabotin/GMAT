function exists = CheckForFile(filename);

% CheckForFile Tests to see if a specified file is in the current folder
%
%  exists = CheckForFile(filename)  Looks for filename in the current
%  folder and returns 1 if it is found, 0 if not.
%
%   INPUTS
%   VARIABLE        SIZE    	DESCRIPTION (Optional/Default)
%   filename        (1x1)       File that is being checked.
%
%   OUTPUTS
%   exists          (1x1)       1 if the file is in the CWD, 0 if not.
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
% Created: 2011/06/24

exists = 0;

data = dir(filename);
sz = size(data);
count = sz(1);

if count > 0
    exists = 1;
end
