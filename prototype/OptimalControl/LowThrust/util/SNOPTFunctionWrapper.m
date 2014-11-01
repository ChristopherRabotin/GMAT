function [C,G] = SNOPTFunctionWrapper(z);
%  Compute the cost and constraints
% Construct the vector of constraints plus the objective function

global iGfun jGvar traj

traj.SetDecisionVector(z);
C = traj.GetCostConstraintFunctions();
J = traj.GetJacobian();
G = snfindG(iGfun,jGvar,J);
