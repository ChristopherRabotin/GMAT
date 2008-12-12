function x = Spacecraft_GetState(Sat,Id)

global ObjectStore

%==========================================================================
%==========================================================================
%------  Get the requested state value defined by Id
%==========================================================================
%==========================================================================

if Id == 1;
    x = [Sat.X Sat.Y Sat.Z Sat.VX Sat.VY Sat.VZ]';
elseif Id == 2;
    x = reshape(Sat.STM,36,1);
elseif Id == 3
    x = Sat.Cd;
elseif Id == 4
    x = Sat.Cr;
elseif Id == 5
    x = Sat.Epoch;
end