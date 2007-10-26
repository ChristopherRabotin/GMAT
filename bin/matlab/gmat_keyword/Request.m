%$Header: /GMAT/dev/cvs/supportfiles/matlab/gmat_keyword/Request.m,v 1.1 2007/08/22 19:15:16 shughes Exp $
function data = Request(channel, var)

%-----------------------------------------------------------
% if computer is 'PCWIN' call dde function to request data
%-----------------------------------------------------------
if (computer == 'PCWIN')
   %data = ddereq(gmatChannel, var); % receive as numeric
   data = ddereq(channel, var, [1,1]); % receive as string
else
   disp('Request(): Only PC windows is supported at this time');
end
