%$Header: /GMAT/dev/cvs/supportfiles/matlab/gmat_keyword/CloseGMAT.m,v 1.1 2007/08/22 19:15:16 shughes Exp $
function CloseGMAT()

global gmatChannel;
%-----------------------------------------------------------
% if computer is 'PCWIN' call dde function to terminate
% conversion with GMAT
%-----------------------------------------------------------
if (computer == 'PCWIN')
   ddeterm(gmatChannel);
else
   disp('CloseGMAT(): Only PC windows is supported at this time');
end

gmatChannel = 0;
%disp('Close;')
