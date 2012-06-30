function [f,g] = OBJ_hs066(x,varargin)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

f = 0.2*x(3) - 0.8*x(1);

g = [ -0.8; 0 ;0.2];
       