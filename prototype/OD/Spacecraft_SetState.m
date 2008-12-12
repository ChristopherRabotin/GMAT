function Sat = Spacecraft_SetState(Sat,Id,x)

global ObjectStore

%==========================================================================
%==========================================================================
%------  Set the requested state value defined by Id
%==========================================================================
%==========================================================================

if Id == 1;

    Sat.X  = x(1,1);
    Sat.Y  = x(2,1);
    Sat.Z  = x(3,1);
    Sat.VX = x(4,1);
    Sat.VY = x(5,1);
    Sat.VZ = x(6,1);
  
elseif Id == 2;
    
    Sat.STM = reshape(x,6,6)';
    
elseif Id == 3
    
    Sat.Cd = x(1,1);
    
elseif Id == 4
    
    Sat.Cr = x(1,1);
    
elseif Id == 4
    
    Sat.Epoch = x(1,1);
    
end