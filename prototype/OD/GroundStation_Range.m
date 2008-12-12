function [range,Htilde] = GroundStation_Range(Sat,GroundStation)

global jd_ref

GS_EF = [GroundStation.X GroundStation.Y GroundStation.Z]';

%==========================================================================
%==========================================================================
%---- Calculate Greenwich Meantime and rotation from fixed to inertial
%               THIS SHOULD BE MOVED SOMEWHERE ELSE!!!!
%==========================================================================
%==========================================================================
currJD   = Sat.Epoch + 2430000;
T_UT1    = (currJD - 2451545)/36525;
GMST     = 67310.54841 + (876600*3600 + 8640184.812866)*T_UT1+ ...
           0.093104*T_UT1^2 - 6.2e-6*T_UT1^3;
       
while GMST < -86400 
    GMST = GMST + 86400;
end

while GMST > 86400 
    GMST = GMST - 86400;
end

GMST = GMST*pi/43200;
R_IF     = R3(GMST);

%==========================================================================
%==========================================================================
%---- Calculate the range vector, the range measurement and the partials
%==========================================================================
%==========================================================================
rangevec       = [Sat.X Sat.Y Sat.Z]' - R_IF'*GS_EF ;

range    = sqrt(rangevec'*rangevec);
Htilde   = rangevec'/range; 