%$Header: /GMAT/dev/cvs/supportfiles/matlab/gmat_keyword/GetGMATVar.m,v 1.1 2007/08/22 19:15:16 shughes Exp $
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
   data = eval(tempdata);
end
