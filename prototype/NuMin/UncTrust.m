function [x,f,G,Converged,OutPut] = uncTrust(func,x0,Options,varargin)

%
%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.


%--------------------------------------------------------------------------
%---------------------------  Initializations -----------------------------
%--------------------------------------------------------------------------

%----- Initialize counters
numfEval = 0;                         %  The number of function evaluations
iter = 0;                          %  The number of optimization iterations
alpha = 1;

%  Determine dimensionality of the problem and evaluate the function and
%  gradient at the initial guess
lengthX            = length(x0);
eyelengthX         = eye(lengthX);
x                  = x0;
%  evaluate function and gradient
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

normG              = norm(G);

%  Write data describing the problem and solution approach
disp([ ' Objective Function: ' func ]);
disp([ ' Number of Variables: ' num2str(lengthX,10)]);
disp([ ' Descent Dirction: ' Options.DescentMethod]);
disp([ ' Derivative Method: ' Options.DerivativeMethod ]);
disp([ ' Step Search Method: ' Options.StepSearchMethod ]);

%--------------------------------------------------------------------------
%---------------  Write Header and data on initial guess ------------------
%--------------------------------------------------------------------------

header = sprintf(['\n                                                         Gradient     Trust      Trust\n',...
    ' Iteration  Func-count       f(x)        Step-size       Magnitude   Accuracy   Radius']);
formatstr = ' %5.0f       %5.0f    %13.6g  %13.6g   %12.3g   %8.3g   %8.3g';
disp(header);

iterdata = sprintf(formatstr,iter,numfEval,f,0,normG);
disp(iterdata);

%--------------------------------------------------------------------------
%----------------------  Perform the Algorithm  ---------------------------
%--------------------------------------------------------------------------

%  Check that we aren't starting at a solution
if normG <= Options.TolGrad
    disp('Algorithm uncmin started at at minimum')
    Converged = 1;
else
    Converged = 0;
end

%----- Initializations
H = eye(lengthX);
maxTrustRadius = 10;
minTrustRadius = -inf;
Delta          = 5;
eta            = 0;

%----- Perform the iteration
while ~Converged && iter <= Options.MaxIter && numfEval <= Options.MaxFunEvals

    %  Increment the counter
    iter = iter + 1;

    %-----  Solve the trust region step
    goodstep = 0;
    tcount   = 0;
    while ~goodstep && tcount <=4;
        
        %  Increment the counter
        tcount = tcount + 1;
        
        %  Solve for the point at the end of the dog leg step
        %  Nocedal and Write Eq. 4.12, 4.13
        pU = -G'*G/(G'*inv(H)*G)*G;
        pB = -H*G;

        %  If the step is outside the trust region, find the intersection with
        %  the trust region
        opt = optimset('Display','Off','TolFun',1e-5);
        if norm(pU) > Delta

            tau = fsolve('FindTrustStep',1,opt,pU,pB,Delta^2,1);
            p = tau*pU;

        elseif norm(pB) > Delta;

            tau = fsolve('FindTrustStep',1.5,opt,pU,pB,Delta^2,2);
            p = pU + (tau - 1)*(pB - pU);
            
        else
            
            p = pB;
            
        end
        
        %  Evaluate function and gradient at step p
        switch Options.DerivativeMethod

            case {'ForwardDiff','BackwardDiff','CentralDiff'}

                fnew                  = feval(func,x+p,varargin{:});
                [Gnew, NumGFuncEvals] = numdiff(func,x+p,fnew,Options,'Gradient',varargin{:});
                numfEval              = numfEval + NumGFuncEvals + 1;

            case 'Analytic'

                [fnew,Gnew]           = feval(func,x+p,varargin{:});
                numfEval              = numfEval + 1;

            otherwise

                disp('DerivativeMethod supplied in Options.DerivativeMethod is not supported')

        end

        %  Compare predicted improvement with actual improvement
        %  Nocedal and Wright Eq. 4.4
        mP  = f + G'*(p) + .5*(p)'*inv(H)*(p);
        ratio = (f - fnew)/(f - mP);
       
        %-----  Adust the trust region radius
        if ratio <= .25
            Delta = max(.25*Delta,minTrustRadius);
        else
            if ratio >= .75 && abs(norm(p) - Delta) < 1e-7;
                Delta = min(2*Delta,maxTrustRadius);
            end
        end

        %-----  Decide to whether to keep this step.  If ratio is negative,
        %then the step resulted in an increase in f!!  If ration is small,
        %( ratio < eta ), then the agreement between the model and actual
        %function is poor and we should reject the step.
        if ratio > eta
            xnew = x + p;
            goodstep = 1;
        elseif ratio < eta && tcount < 4
            goodstep = 0;
            %disp(['Trust Step Rejected    '  num2str(Delta) '  ' num2str(ratio) ]);
        else
            xnew = x + p;
        end
        
    end
     
%     % Check that we are actually using a descent direction
%     if p'*G > 0
%         disp('Warning:  Search Direction is not a descent direction in uncmin')
%         disp('Optimization did not succeed')
%         Converged = 0;
%         OutPut.iter = iter;
%         OutPut.numfEval = numfEval;
%         eig(inv(H));
%         keyboard
%         H = eye(lengthX)/lengthX;
%     end

    %  Output iterate data
    iterdata = sprintf(formatstr,iter,numfEval,fnew,norm(p),norm(Gnew),ratio, Delta);
    disp(iterdata);

    %   Check convergence
    [Converged] = CheckConvergence(Gnew, f, fnew, x, xnew, norm(p), Options);

    %   Update state data for next iteration
    if Converged == 0

        y = Gnew - G;
        s = xnew - x;

        %  Nocedal and Wright Eq. 8.20;
        if iter == 1
            H = y'*s/(y'*y)*eyelengthX;
        end

        switch Options.DescentMethod

            case 'BFGS'

                %  From Nocedal and Wright Eq. 8.16
                %  Update the inverse Hessian.
                rho = 1/(y'*s);
                H = ( eyelengthX - rho*s*y')*H*(eyelengthX - rho*y*s') + rho*s*s';

            case 'DFP'

                %  From Nocedal and Wright Eq. 8.14
                %  Update the inverse Hessian.
                H = H - H*y*y'*H/(y'*H*y) + s*s'/(y'*y);

            case 'SR1'

                %  Update condition is from Nocedal and Wright Eq. 8.26
                prod = (s - H*y);
                if abs(s'*prod) > 1e-12*norm(s)*norm(prod)

                    % From Nocedal and Wright Eq. 8.25
                    H = H + prod*prod'/(prod'*y);

                end

        end


    end

    %   Update the state components for the next iteration
    x = xnew; f = fnew; G = Gnew; 

end

%  If did not converge, determine why and write to screen
if Converged == 0

    if numfEval == Options.MaxFunEvals
        field = sprintf(['\n Optimization Failed \n' ...
            ' Solution was not found within maximum number \n of allowed function evaluations \n']);
        disp(field)
    else
        field = sprintf(['\n Optimization Failed \n' ...
            ' Solution was not found within maximum number \n of allowed iterations \n']);
        disp(field)
    end

end

OutPut.iter = iter;
OutPut.numfEval = numfEval;

%==========================================================================
function Converged = CheckConvergence(Gnew, f, fnew, x, xnew, alpha, Options)

if norm(Gnew) < Options.TolGrad

    Converged = 1;

    field = sprintf(['\n Optimization Terminated Successfully \n' ...
        ' Magnitude of gradient less than Options.tol \n']);
    disp(field)

elseif ( abs(fnew - f) < Options.TolF )

    Converged = 2;

    field = sprintf(['\n Optimization Terminated Successfully \n' ...
        ' Absolute value of function improvement \n is less than tolerance']);
    disp(field)

elseif (norm(x - xnew) < Options.TolX)

    Converged = 3;

    field = sprintf(['\n Optimization Terminated Successfully \n' ...
        ' Change in x is less than tolerance \n']);
    disp(field)

elseif alpha < Options.TolStep

    Converged = 4;

    field = sprintf(['\n Optimization Terminated Successfully \n' ...
        ' Step length is less than tolerance \n']);
    disp(field)

else

    Converged = 0;

end