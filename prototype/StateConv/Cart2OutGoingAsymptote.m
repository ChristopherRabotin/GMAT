function [outGoingAsymptote] = Cart2OutGoingAsymptote(cartState,mu)

% outGoingAsymptote = [C3 radPer INC RLA DLA TA];

%  Set mu.  If not provided assume Earth.
if nargin < 2
    mu = 398600.4418;
end

%%  Compute variables needed elsewhere
twopi = 2*pi;
rv   = cartState(1:3,1);
rmag = norm(rv);
vv   = cartState(4:6,1);
vmag = norm(vv);
hv   = cross(rv,vv);
hmag = norm(hv);
nv   = cross([0 0 1]',hv);  %  vector in direction of RAAN,  zero vector if equatorial orbit
n    = norm(nv);
ev   = (vmag*vmag - mu/rmag)*rv/mu - dot(rv,vv)*vv/mu;
ECC  = norm(ev);

%%  Compute C3 and SMA
C3     = vmag * vmag - 2 * mu / rmag;
Energy = vmag * vmag/2 - mu / rmag;
SMA    = -mu/2/Energy;

%%  Compute Asymptote vector
%  If orbit is hyperbolic, use asymptote
if C3 > 0
    fac1  = 1/(1 + C3*(hmag*hmag)/(mu*mu));
    svhat = fac1*(sqrt(C3)/mu*cross(hv,ev) - ev);
else
    %
    %  The two options below are really hacks, the asymptote doesn't exist.  However,
    %  having a definition that is unique and has an inverse computation is
    %  useful, especially when numeric solvers change regimes during
    %  iterative processes. - SPH
    %
    %  If orbit is elliptic, use apsides vector
    if ECC > 1e-7
        svhat = -ev/ECC;
        disp('Warning in Cart2OutGoingAsymptote: Orbit is elliptic so using Apsides vector for asymptote.')
        %  If orbit is circular, throw an exception
    else
        outGoingAsymptote = [];
        disp('Error in Cart2OutGoingAsymptote: Cannot convert from Cartestion to Asymptote because orbit is circular.');   
    return
    end
end

%%  Compute other elements
DLA    = asin(svhat(3));
RLA    = atan2(svhat(2),svhat(1));
radPer = SMA*(1 - ECC);
TA     = acos(dot(ev/ECC,rv/rmag));
INC    = acos(dot(hv/hmag,[0 0 1]'));
if RLA < 0
    RLA = RLA + 2*pi;
end

if ECC >= 1
   maxTA = acos(-1/ECC);
else
   maxTA = pi;
end
%  TODO:  determine how to handle ev = [0 0 0];
AOP    = acos(dot(nv/n,ev/ECC));
if ( ev(3) < 0 )
   AOP = twopi - AOP;
end
cosAZI = -cos(maxTA + AOP) * sin(INC) / cos(DLA);
velAzimuth = acos(cosAZI);

%%  Assemble the output
outGoingAsymptote = [C3 radPer  RLA DLA TA velAzimuth];