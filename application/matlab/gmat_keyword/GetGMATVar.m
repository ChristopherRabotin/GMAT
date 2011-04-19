%$Header: /GMAT/dev/cvs/supportfiles/matlab/gmat_keyword/GetGMATVar.m,v 1.1 2007/08/22 19:15:16 shughes Exp $
% Copyright (c) 2002 - 2011 United States Government as represented by the
% Administrator of the National Aeronautics and Space Administration.
% All Other Rights Reserved.
function data = GetGMATVar(var)

%--------------------------------------
% call dde function to request data
%--------------------------------------
global gmatChannel;
disp(['Get GMAT::' var]);

if (gmatChannel == 0) 
   disp('channel is not valid');
else
   tempdata = Request(gmatChannel, var);
   try % Extract numeric data
       data = eval(tempdata); 
   catch % Extract string data
       tempdata2 = tempdata(2:size(tempdata,2)-1); % Strip brackets from tempdata
       data = tempdata2;
   end
end
