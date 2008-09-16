function [Flow,Fupp,iGfun,jGvar] = prepSNOPT(objname,x,d)

%
%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

%-----  Call the constraint function to determine constraint dimensions
conname                     = ['CON_' objname];
[ci,ce,Ji,Je,ciGfun,cjGvar] = feval(conname,x);

%-----  Call the objective function  
objname       = ['OBJ_' objname];
[f,g, fiGfun, fjGvar] = feval(objname,x);

%-----  Detemine constraint dimensions
mNLI = size(ci,1);              %  Number of nonlin. ineq. constraints
mNLE = size(ce,1);              %  Number of nonlin. eq. constraints
mNL  = mNLI + mNLE;             %  

%-----  Set upper and lower bounds on functions
Flow  = [-inf;zeros(mNLE+mNLI,1)];
Fupp  = [ inf;zeros(mNLE,1);inf*ones(mNLI,1)];

%-----  Seup up vector of nonzero elements of gradient and Jacobian 
iGfun = [fiGfun;ciGfun+1];
jGvar = [fjGvar;cjGvar];







