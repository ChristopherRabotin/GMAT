
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   conversion Alternate Equinoctial State to Equinoctial State        
%   by HYKim, CBNU                                             
%   modified on May,22,2013         
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



function Equinoctial = AltEq2Eq(AltEquinoctial)

% If mu in not provided, use the mu for Earth
if nargin < 2
   mu = 398600.4415;
end

% Initializations
SMA = AltEquinoctial(1,1);
e1 = AltEquinoctial(2,1);
e2 = AltEquinoctial(3,1);
e3 = AltEquinoctial(4,1);
e4 = AltEquinoctial(5,1);
e5 = AltEquinoctial(6,1);

% Sanity check input
if max(size(AltEquinoctial))~= 6
    Equinoctial = [];
    disp('Error: Alternate equinoctial state have six elements')
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
h   = e2;
k   = e1;
p   = e3/c;
q   = e4/c;
meanLon = e5;

% Output
Equinoctial = real([sma h k p q meanLon]'); 



