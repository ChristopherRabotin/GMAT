%$Header: /GMAT/dev/cvs/supportfiles/matlab/gmat_keyword/Advise.m,v 1.1 2007/08/22 19:15:16 shughes Exp $
% Copyright (c) 2002 - 2011 United States Government as represented by the
% Administrator of the National Aeronautics and Space Administration.
% All Other Rights Reserved.
function data = Advise(channel, item, var)

%-----------------------------------------------------------
% if computer is 'PCWIN' call dde function to request data
%-----------------------------------------------------------
if (computer == 'PCWIN')
   disp('calling ddeadv');
   ddeadv(channel, item, 'disp(var)', var, [1,1]); % receive as string
   var
else
   disp('Request(): Only PC windows is supported at this time');
end
