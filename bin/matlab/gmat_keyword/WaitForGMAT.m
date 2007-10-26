%$Header: /GMAT/dev/cvs/supportfiles/matlab/gmat_keyword/WaitForGMAT.m,v 1.1 2007/08/22 19:15:16 shughes Exp $
%------------------------------------------------------------------------------
%                               WaitForGMAT.m
%------------------------------------------------------------------------------
% GMAT: Goddard Mission Analysis Tool
%
% ** Legal **
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

if (gmatChannel == 0) 
   disp('channel is not valid');
else
   var = 'RunState';
   pause(.1);
   continueWait = true;
   
   while(continueWait)
      
      gmatRunState = Request(gmatChannel, var);
      
      if (isfloat(gmatRunState))
         CloseGMAT;
         OpenGMAT;
         continueWait = true;
      elseif(ischar(gmatRunState))
         if (strcmp(gmatRunState,'Idle'))
            continueWait = false;
         else
            continueWait = true;
         end
      end
      
      pause(.1);
      
   end
end
