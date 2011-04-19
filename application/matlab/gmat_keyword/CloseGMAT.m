%$Header: /cygdrive/p/dev/cvs/matlab/gmat_keyword/CloseGMAT.m,v 1.3 2005/12/22 19:17:12 jgurgan Exp $
% Copyright (c) 2002 - 2011 United States Government as represented by the
% Administrator of the National Aeronautics and Space Administration.
% All Other Rights Reserved.
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
