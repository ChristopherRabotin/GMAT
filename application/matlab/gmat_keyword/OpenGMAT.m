% Copyright (c) 2002 - 2011 United States Government as represented by the
% Administrator of the National Aeronautics and Space Administration.
% All Other Rights Reserved.
function OpenGMAT()

global gmatService;
global gmatTopic;
global gmatChannel;

gmatService = '4242';
gmatTopic = 'GMAT-MATLAB';

%-----------------------------------------------------------
% if computer is 'PCWIN' call dde function to initialize
% conversation with GMAT
%-----------------------------------------------------------
%if strcmp(computer,'PCWIN')
if ispc
   gmatChannel = ddeinit(gmatService, gmatTopic);
else
%   disp('OpenGMAT(): Only PC windows is supported at this time');
   CallGMAT('Open','');   
   return;
end

if (gmatChannel == 0)
   disp('GMAT Server has not started. Please start the server first!');
else
   CallGMAT('Open', '');
end
