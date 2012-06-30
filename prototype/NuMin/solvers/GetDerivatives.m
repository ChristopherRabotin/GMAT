function [f,G,numfEval] = GetDerivatives(func,x,Options,varargin)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.


numfEval = 0;
switch Options.DerivativeMethod

    case {'ForwardDiff','BackwardDiff','CentralDiff'}

        f                  = feval(func,x,varargin{:});
        [G, NumGFuncEvals] = numdiff(func,x,f,Options,'Gradient',varargin{:});
        numfEval           = numfEval + NumGFuncEvals + 1;

    case 'Analytic'

        [f,G]              = feval(func,x,varargin{:});
        numfEval           = numfEval + 1;
         
    otherwise

        disp('DerivativeMethod supplied in Options.DerivativeMethod is not supported')

end