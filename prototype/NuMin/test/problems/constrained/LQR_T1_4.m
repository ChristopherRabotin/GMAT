function Data = LQR_T1_4()

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

Data.x0  = [-.1 -1 .1]';
Data.lb  = -inf*ones(3,1);
Data.ub  =  inf*ones(3,1);
Data.A   = [];
Data.b   = [];
Data.Aeq = [];
Data.beq = [];
Data.xstar = [.6 .8 0]';
Data.fstar = -.8;