function  Propagator = Propagator_SteptoEpoch(Propagator,PropEpoch)

global ObjectStore

%==========================================================================
%==========================================================================
%---- Step to the requested epoch
%==========================================================================
%==========================================================================

%  Perform the Propagation
[t,X] = Propagator_ODE78('Propagator_ODEModel', 0, (PropEpoch - Propagator.Epoch)*86400, Propagator.PSV, 1e-13 , [], [], Propagator); 

%  Extract state from ephemeris output and update the propagator
numSteps = size(t,1);
X        = X(numSteps,:)';
Propagator.Epoch = Propagator.Epoch + t(numSteps)/86400;
Propagator.PSV   = X;


%==========================================================================
%==========================================================================
%---- Populate the propagated objects with the new state data
%==========================================================================
%==========================================================================

xCounter = 1;
for i = 1:size(Propagator.PropObjects,2)

    %  Get the ith object from the ObjectStore
    this = Propagator.PropObjects(i);
    Obj  = ObjectStore.Objects{this};
    
    %  KLUDGE TO UPDAT THE EPOCH OF OBJECT.
    Obj.Epoch = Propagator.Epoch;
    
    %----- Loop over the parameters estimated on the ith object
    for j = 1:size(Propagator.PropParamId{i},2)

        %-----  Determine the size of the current state element
        paramId = Propagator.PropParamId{i}(j);
        if paramId == 1
             sizeCurrElement = 6;
        elseif paramId == 2
            sizeCurrElement = 36;
        elseif paramId == 3 || paramId == 4
            sizeCurrElement = 1;
        end
       
        %-----  Extract the current state element from the total state
        xElement = X(xCounter:xCounter+sizeCurrElement-1);
        
        %----- Add spacecraft related state components
        if Propagator.PropObjects(i) == 1
            Obj   = Spacecraft_SetState(Obj, paramId,xElement);
        end

        xCounter = xCounter + sizeCurrElement;

    end
    
    %  Update the object store
    ObjectStore.Objects{i} = Obj;

end



