%$Header: /cygdrive/p/dev/cvs/matlab/gmat_keyword/WaitForGMAT.m,v 1.3 2006/08/28 17:38:29 wshoan Exp $
%------------------------------------------------------------------------------
%                               WaitForGMAT.m
%------------------------------------------------------------------------------
% GMAT: Goddard Mission Analysis Tool
%
% ** Legal **
%
% Copyright (c) 2002 - 2011 United States Government as represented by the
% Administrator of the National Aeronautics and Space Administration.
% All Other Rights Reserved.
%
% Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
% number S-67573-G
%
% Author: Linda Jun
% Created: 2005/08/02
%
% Matlab function to wait for GMAT run completion.
%
%------------------------------------------------------------------------------
function WaitForGMAT()

global gmatChannel;
global gmatRunState;

% if (gmatChannel == 0) 
if (~isunix && gmatChannel == 0) 
   disp('channel is not valid');
else
   var = 'RunState';
   if isunix
      pause(5);
   else
      pause(1);
   end
   
   continueWait = true;
   
   while(continueWait)
      
      gmatRunState = Request(gmatChannel, var)
      
      %% On computer PICWIN:
      %% Close/Open to get new channel if String value was not received from Request().
      %% It could mean the Server is not responding to new Request so it has timed out.
      %% This happens when building and running a long script.
      if (isfloat(gmatRunState))
         if (~isunix)
            CloseGMAT;
            OpenGMAT;
         end
         continueWait = true;
      elseif(ischar(gmatRunState))
         if (strcmp(gmatRunState,'Idle'))
            continueWait = false;
         else
            continueWait = true;
         end
      end
      
      if isunix
         pause(5);
      else
         pause(1);
      end 

   end
end
