function [C,G] = SNOPTFunctionWrapper(z);

global iGfun jGvar traj

%  Configure automatic differentiation using intlab.
zad = gradientinit(z);
traj.SetDecisionVector(zad);

%  Compute the cost and constraints
constraintVector = traj.GetContraintVector();
costFunction     = traj.GetCostFunction();

% Construct the vector of constraints plus the objective function
F = [costFunction; constraintVector];

% Extract gradient information
C = F.x;
J = F.dx;
G = snfindG(iGfun,jGvar,J);
