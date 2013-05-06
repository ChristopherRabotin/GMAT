
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   conversion Equinoctial elements to Nonsinular Keplerian      %    
%   by HYKim, CBNU                                               %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



function Nonsingular = Equinoctial2Nonsingular(equinoct,mu)

% If mu in not provided, use the mu for Earth
if nargin < 2
   mu = 398600.4415;
end

% Initializations
sma = equinoct(1,1);
h   = equinoct(2,1);
k   = equinoct(3,1);
p   = equinoct(4,1);
q   = equinoct(5,1);
meanLon = equinoct(6,1);

% Sanity check input
if max(size(equinoct))~= 6
    Nonsingular = [];
    disp('Error: Equinoctial elements have six elements')
    return
end

% Using the relationship between classical and equnoctial elements
INC = 2*atan(sqrt(p^2+q^2));

    if(INC == pi)
    disp('Warning: Conversion results is near singularity that occurs when INC = 180(deg).');
     return
    end
    
    c = cos(INC/2);  

% Find the Nonsingular Keplerian
SMA = sma;
e1 = h;
e2 = k;
e3 = p*c;
e4 = q*c;
e5 = meanLon;

% Output
Nonsingular = real([SMA e1 e2 e3 e4 e5]');     



