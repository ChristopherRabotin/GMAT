function [xdot,Amat] = ODEmodel_PointMasses(t,x,Propagator,PropSTM);

global SolarSystem jd_ref

%  Determine if the STM is being propagated
if size(x,1) == 6
    PropSTM = 0;
elseif size(x,1) == 42
    PropSTM = 1;
end

%  Calculate useful terms for later
rv     = x(1:3,1);
vv     = x(4:6,1);
r      = sqrt(rv'*rv);
r3     = r^3;
r5     = r^5;
eye3   = eye(3);
zero3  = zeros(3);
muCB   = SolarSystem.Mu(Propagator.FM.CentralBodyIndex);
mubyr3 = muCB/r3;

%  Calulate terms associated with central body
xdot(1:3,1) = vv;
xdot(4:6,1) = -mubyr3*rv;

if PropSTM
    A  = zero3;
    B  = eye3;
    C  = -mubyr3*eye3 + 3*muCB*rv*rv'/r5;
    D  = zero3;
else
    A  = zero3;
    B  = zero3;
    C  = zero3;
    D  = zero3;
end

%  Calulate terms associated with point mass perturbations
if size(Propagator.FM.PointMassIndeces,2) > 1
    xdot_PM = zeros(3,1);
    C_PM    = zeros(3,3);
    [DeltaAT] = MJD2TimeCoeff(jd_ref);
    DeltaTT   = DeltaAT + 32.184;
    jd_tt     = jd_ref + (DeltaTT + t)/86400;
    for i = 1:size(Propagator.FM.PointMassIndeces,2)

        %  If the point mass is not the central body!!
        if Propagator.FM.PointMassIndeces{i} ~= Propagator.FM.CentralBodyIndex

            muk     = SolarSystem.Mu(Propagator.FM.PointMassIndeces{i});
            rvk     = pleph(jd_tt,Propagator.FM.PointMassIndeces{i},Propagator.FM.CentralBodyIndex,1);
            rvkmr   = rvk - rv;
            rvk3    = norm(rvk)^3;
            rvkmr3  = norm(rvkmr)^3;
            rvkmr5  = norm(rvkmr)^5;
            xdot    = xdot  + muk*(rvkmr/rvkmr3 - rvk/rvk3);

            if PropSTM
                C    = C + muk*( - 1/rvkmr3*eye3 + 3*rvkmr*rvkmr'/rvkmr5);
            end

        end

    end
end

%  Now construct Amat
Amat = [A B; C D];



