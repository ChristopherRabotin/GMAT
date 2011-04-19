%$Header$
%------------------------------------------------------------------------------
%                               WaitForCallbackResults.m
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
% Author: Wendy Shoan
% Created: 2006.09.21
%
% Matlab function to wait for callback results from GMAT.
%
%------------------------------------------------------------------------------
function WaitForCallbackResults()

global gmatChannel;
global callbackResults;

% if (gmatChannel == 0) 
if (~isunix && gmatChannel == 0) 
   disp('channel is not valid');
else
   var = 'CallbackResults';
%   if isunix
%      pause(1);
%   else
%      pause(1);
%   end

   continueWait = true;
   
   while(continueWait)
      
      callbackResults = Request(gmatChannel, var)
      
      if (isfloat(callbackResults))
         %% Skip Close/Open to stay with the same channel
         %%CloseGMAT;
         %%OpenGMAT;
         continueWait = true;
      elseif(ischar(callbackResults))
      %   if (strcmp(callbackResults,'Completed'))
            continueWait = false;
      %   else
      %      continueWait = true;
      %   end
      end
      
%      if isunix
%         pause(1);
%      else
%         pause(1);
%      end 

   end
end
