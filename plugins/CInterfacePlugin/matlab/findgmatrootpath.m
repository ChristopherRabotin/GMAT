function root_path = findgmatrootpath
% FINDGMATROOTPATH Extracts GMAT's ROOT_PATH from gmat_startup_file.txt

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
% Author: Russell Carpenter, NASA GSFC
% Created: 2011/08/08

fid = fopen('gmat_startup_file.txt','r');
if fid == -1
    error('gmat_startup_file not found on current path')
else
    gsfl = '';
    while ~strncmp(gsfl,'ROOT_PATH',9)
        gsfl = fgetl(fid);
    end
    fclose(fid);
    ps = strfind(gsfl,'=');
    root_path = strtrim(gsfl(ps+1:end));
end