%$Header: /cygdrive/p/dev/cvs/matlab/gmat_keyword/CloseGMAT.m,v 1.3 2005/12/22 19:17:12 jgurgan Exp $
function CloseGMAT()

global gmatChannel;
%-----------------------------------------------------------
% if computer is 'PCWIN' call dde function to terminate
% conversation with GMAT
%-----------------------------------------------------------
if strcmp(computer, 'PCWIN')
   ddeterm(gmatChannel);
elseif isunix
   CallGMAT('Close','');
else 
   disp('CloseGMAT(): Unknown platform is not supported at this time');
end

gmatChannel = 0;
disp('Close;')
