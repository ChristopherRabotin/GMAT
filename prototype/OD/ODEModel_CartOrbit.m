function [xdot] = ODEModel_CartOrbit(t,x,PropSTM,Propagator)


%  Determine if the STM is being propagated
xdot = zeros(6,1); 
if PropSTM == 1;
   A    = zeros(6,6);
   phi  = reshape(x(7:42),6,6)';
end
    
%  Call point mass model
[xdot_plus,A_plus] = ODEmodel_PointMasses(t,x,Propagator,PropSTM);
xdot = xdot + xdot_plus;
A    = A + A_plus;

switch Propagator.FM.SRP

    case 'On'
        
        [xdot_plus,A_plus] = deriv_SRP(t,x,Propagator,PropSTM);
        xdot = xdot + xdot_plus;
        A    = A + A_plus;

end

if PropSTM == 1
    phidot   = A*phi;
    xdot  = [xdot;reshape(phidot',36,1)];
end