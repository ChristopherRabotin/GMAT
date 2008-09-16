function [v,Beta] = householder(x)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.


[n,m] = size(x);
v = [];
Beta = [];

if m > 1
    disp('Error:  householder.m requires a column vector');
    return
end

sigma = x(2:n)'*x(2:n);
v = [1;x(2:n)];

if sigma == 0
    Beta = 0;
else
    mu = sqrt(x(1)^2 + sigma);
    if x(1) <= 0
        v(1) =x(1) - mu;
    else
        v(1) = -sigma/(x(1) + mu);
    end
    Beta = 2*v(1)^2/(sigma + v(1)^2);
    v    = v/v(1);
end

