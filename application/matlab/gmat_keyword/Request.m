%$Header: /cygdrive/p/dev/cvs/matlab/gmat_keyword/Request.m,v 1.3 2005/12/22 19:17:12 jgurgan Exp $
% Copyright (c) 2002 - 2011 United States Government as represented by the
% Administrator of the National Aeronautics and Space Administration.
% All Other Rights Reserved.
function data = Request(channel, var)

%-----------------------------------------------------------
% if computer is 'PCWIN' call dde function to request data
%-----------------------------------------------------------
if strcmp(computer,'PCWIN')
   %data = ddereq(gmatChannel, var); % receive as numeric
   data = ddereq(channel, var, [1 1]); % receive as string, default timeout of 3 sec
elseif isunix 
   data = SendGMAT('Request',var);
else
   disp('Request(): unknown platform is not supported at this time');
end
