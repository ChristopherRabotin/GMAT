function X = GetState(Sat,Id)

switch Id
    
    case 1
        
        X = [Sat.X Sat.Y Sat.Z Sat.VX Sat.VY Sat.VZ]'; 
        
    case 2
        
        X = Sat.Cd;
        
    case 3
        
        X = Sat.Cr;
        
end