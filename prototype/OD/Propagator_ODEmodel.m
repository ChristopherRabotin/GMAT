function [xdot] = Propagator_ODEmodel(t,x,ForceModel)

global SolarSystem


%  Loop over all elements in the state vector and call the appropriate ODE
%  model for each elements.  The ODEmap constains the data that describes
%  what is contained in the state vector.
%
%  1 :  Spacecraft cartesian state
%  2 :  Spacecraft STM
%  3 :  Spacecraft Cr
%  4 :  Spacecraft Cd
%  
ODEmap          = ForceModel.ODEmap;
NumPropElements = size(ODEmap.Size,2);
NumStates       = size(x,1);

i = 1;
xCounter = 1;
while i < NumPropElements
 
    % If element is the cartesian state with or w/o STM
    
    if ODEmap.Type(i) == 1;

        %  Determine if STM is being propagated 
        PropSTM = 0;
        sizeCurrElement = 6;
        if i+1 <= NumPropElements  
            if ODEmap.Type(i+1) == 2
                PropSTM = 1;
                sizeCurrElement = 42;
            end
        end
        
        %  Call the ODEmodel for orbit cartesian state
        xCurrElement = x(xCounter:xCounter+sizeCurrElement-1);
        xdot = ODEModel_CartOrbit(t,xCurrElement,PropSTM,ForceModel);
        
        i = i + 2;
        xCounter = xCounter + sizeCurrElement;
        
    end
    
    
end

