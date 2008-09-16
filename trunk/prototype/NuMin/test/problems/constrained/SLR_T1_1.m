function Data = SLR_T1_1()

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

Data.x0  = [2 2 2 2 2]';
Data.lb  = []; 
Data.ub  = [];
Data.A   = [];
Data.b   = [];
Data.Aeq = [];
Data.beq = [];
Data.xstar = [-33 11 27 -5 11]'/43;
Data.fstar = 176/43;