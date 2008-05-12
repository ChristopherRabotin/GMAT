%$Header: /GMAT/dev/cvs/supportfiles/matlab/gmat_keyword/Poke.m,v 1.1 2007/08/22 19:15:16 shughes Exp $
function Poke(channel, dataType, dataStr)

%-----------------------------------------------------------
% if computer is 'PCWIN' call dde function to send string data
%-----------------------------------------------------------
%if (computer == 'PCWIN')
if ispc
   ddepoke(channel, dataType, dataStr);
else
   disp('Poke(): Only PC windows is supported at this time');
end
