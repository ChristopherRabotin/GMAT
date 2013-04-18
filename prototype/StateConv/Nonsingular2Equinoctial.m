
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   conversion Nonsinular Keplerian to Equinoctial elements      %    
%   by HYKim, CBNU                                               %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



function equinoct = Nonsingular2Equinoctial(Nonsingular,mu)

% If mu in not provided, use the mu for Earth
if nargin < 2
   mu = 398600.4415;
end

% Initializations
SMA = Nonsingular(1,1);
e1 = Nonsingular(2,1);
e2 = Nonsingular(3,1);
e3 = Nonsingular(4,1);
e4 = Nonsingular(5,1);
e5 = Nonsingular(6,1);

% Sanity check input
if max(size(Nonsingular))~= 6
    equinoct = [];
    disp('Error: Nonsingualr Keplerian have six elements')
    return
end

% Using the relationship between classical and equnoctial elements
INC = 2*asin(sqrt(e3^2+e4^2));

    if(INC == pi)
    disp('Warning: Conversion results is near singularity that occurs when INC = 180(deg).');
     return
    end
    
    c = cos(INC/2);
        
% Find the equnoctial elements
sma = SMA;
h   = e1;
k   = e2;
p   = e3/c;
q   = e4/c;
meanLon = e5;

% Output
equinoct = real([sma h k p q meanLon]); 



